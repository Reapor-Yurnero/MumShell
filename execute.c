//
// Created by Reapor Yurnero on 03/10/2018.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <errno.h>
/* Not technically required, but needed on some UNIX distributions */
#include <sys/types.h>
#include <sys/stat.h>
#include "parse.h"
#include "execute.h"

int executejobs(jobs_t* jobs)
{
    int in = STDIN_FILENO, out;
    for (int i = 0;i < jobs->p_num;++i)
    {
//        int rtn = verifyjobs(jobs);
//        if (rtn != 0) return rtn;
        process_t * p = &jobs->process[i];
        char* command = NULL;
        if (p->argc > 0) command = p->argv[0];
        else continue; // we ensure that valid (argc>=1) process will be executed
        if (strcmp(command, "exit") == 0) return -100;
        if (strcmp(command, "jobs") == 0) return -50;
        if (strcmp(command, "cd") == 0) {
            if (p->argc < 2) continue;
            else if (chdir(p->argv[1]) < 0) {
                //TODO: improve the error
                perror(p->argv[1]);
            }
            continue;
        }
        int fd[2];
        pipe(fd);
        out = (i == jobs->p_num - 1) ? STDOUT_FILENO : fd[1];
        // printf("executing process[%d]\n", i);
        int rtn = executeprocess(p, in, out, fd, jobs);
        in = fd[0];
        close(fd[1]); // ???
        //close(fd[0]);
        if (rtn != 0) {
            reapechildren(jobs);
            return rtn;
        }
    }
    return 0;
}

int executeprocess(process_t* p, int in, int out, int* fd, jobs_t* jobs) {
    pid_t pid = fork();
    if (pid < 0) {
        perror("failed to fork subprocess");
        return -1;
    }
    else if (pid == 0)
    {// subprocess
        //printf("childprocess in\n");
        close(fd[0]); // subprocess always don't need new pipe for read
        //printf("pipe connect %s\n",p->argv[0]);
        int stdoutbk = dup(STDOUT_FILENO);
        if (in != STDIN_FILENO) {
            if (p->inMode == FILEIN) {
                dup2(stdoutbk,STDOUT_FILENO);
                printf("error: duplicated input redirection\n");
                close(in);close(fd[1]);close(STDIN_FILENO);close(stdoutbk);
                freejobs(jobs);
                exit(-1);
            }
            close(STDIN_FILENO);
            //stdinbk = dup(STDIN_FILENO);
            dup2(in, STDIN_FILENO);
            close(in);
            // printf("%s stdin dup connected in=%d\n",p->argv[0],in);
        }
        if (out == STDOUT_FILENO) close(fd[1]);// printf("%d %d\n",fd[0],fd[1]);
        else { // out == fd[1]
            if (p->outMode == FILEOUT || p->outMode == FILEAPPEND) {
                dup2(stdoutbk,STDOUT_FILENO);
                printf("error: duplicated output redirection\n");
                close(out);close(stdoutbk);
                freejobs(jobs);
                exit(-1);
            }
            // printf("%s out=%d\n",p->argv[0], out);
            close(STDOUT_FILENO);
            //stdoutbk = dup(STDOUT_FILENO);
            dup2(out, STDOUT_FILENO);
            // printf("stdout dup connected %s\n",p->argv[0]);
            close(out); // close fd[1]
        }
        // deal with redirection
        // printf("redirection %s\n",p->argv[0]);
        int ifile = -1, ofile = -1;
        if (p->inMode == FILEIN) {
            ifile = open(p->inFile, O_RDONLY);
            if (ifile < 0) {
                int errnum = errno;
                if (errnum == EACCES) printf("%s: Permission denied\n", p->inFile);
                else perror(p->inFile);
                fflush(stdout);
                //return -1;
                close(ifile);
                freejobs(jobs);
                exit(ifile);
            }
            else dup2(ifile, STDIN_FILENO);
        }
        if (p->outMode == FILEOUT) {
            ofile = open(p->outFile, O_WRONLY | O_CREAT | O_TRUNC, 0666);
            if (ofile < 0) {
                // ugly code here for oj
                //int errnum = errno;
                //printf("%d\n",errnum);
                printf("%s: Permission denied\n", p->outFile);
                //else perror(p->outFile);
                fflush(stdout);
                // return -1
                close(ofile);
                freejobs(jobs);
                exit(ofile);
            }
            else dup2(ofile, STDOUT_FILENO);
        }
        else if (p->outMode == FILEAPPEND) {
            ofile = open(p->outFile, O_WRONLY | O_CREAT | O_APPEND, 0666);
            if (ofile < 0) {
                perror(p->outFile);
                //return -1;
                close(ofile);
                freejobs(jobs);
                exit(ofile);
            }
            else dup2(ofile, STDOUT_FILENO);
        }
        // execute command
        // printf("start executing command %s\n",p->argv[0]);
        if (strcmp(p->argv[0],"pwd")==0) {
            char pwdresult[MAX_ARG_LEN] = {0};
            getcwd(pwdresult, MAX_ARG_LEN);
            printf("%s\n", pwdresult);
        }
        else {
            int rtn = execvp(p->argv[0],p->argv);
            // printf("command finished\n");
            if (rtn < 0) {
                dup2(stdoutbk,STDOUT_FILENO);
                // int errnum = errno;
                // if (errnum == ENOENT)
                printf("%s: command not found\n",p->argv[0]);
                //else perror(p->argv[0]);
                fflush(stdout);
                close(stdoutbk);
                freejobs(jobs);
                //
                exit(rtn);
            }
        }
        if (p->outMode == FILEOUT || p->outMode == FILEAPPEND)
            close(ofile);
        if (p->inMode == FILEIN) close(ifile);
        // end the child process anyway
        // printf("childprocess end\n");
        freejobs(jobs);
        exit(0);
    }
    else { // parent process
        p->pid = pid;
//        if (out != STDOUT_FILENO && (p->outMode == FILEOUT || p->outMode == FILEAPPEND)) {
//            printf("error: duplicated output redirection\n");
//            return -1;
//        }
//        if (in != STDIN_FILENO && p->inMode == FILEIN) {
//            printf("error: duplicated input redirection\n");
//            return -1;
//        }
        if (!jobs->background) {
            if (out == STDOUT_FILENO) { // the last child process
                // wait all child process to terminate at the end of jobs
                // int wstatus;
                // printf("parent waiting..\n");
                // for (int j = 0; j < jobs->p_num;++j) wait(&wstatus);
                // (void)pnum;
                // while (waitpid(-1,&wstatus,0) != -1);
                // usleep(10000);
                // printf("parent ends waiting\n");
                reapechildren(jobs);
                for (int i = 3;i <= fd[1];++i) {
                    close(i);
                }
            }
        }
        else {
            //printf("nowait for %d\n", pid);
        }
        // else printf("parent nothing to do\n");
    }
    return 0;
}

void reapechildren(jobs_t* jobs) {
    for (int i = 0; i < jobs->p_num; ++i) {
        if (jobs->process[i].pid != -1) {
            int wstatus;
            waitpid(jobs->process[i].pid, &wstatus, 0);
        }
    }
}

void showjobslist(jobs_t* jobslist, int size) {
    for (int i = 0; i < size; ++i) {
        jobs_t* jobs = &jobslist[i];
        char status[20] = {0};
        strcpy(status,"done");
        for (int j = 0; j < jobs->p_num;++j) {
            int wstatus;
            //printf("%s status\n",jobs->process[j].argv[0]);
            if (waitpid(jobs->process[j].pid, &wstatus, WNOHANG) == 0)
            {strcpy(status,"running");}
        }
        printf("[%d] %s %s\n",i+1,status,jobs->name);
    }
}

int verifyjobs(jobs_t* jobs) {
    for (int i = 0;i < jobs->p_num;++i) {
        if (jobs->process[i].pid != -1) {
            int wstatus;
            if (waitpid(jobs->process[i].pid, &wstatus, WNOHANG) == 0) continue;
            //printf("%d %d\n",i,wstatus);
            if (WIFEXITED(wstatus)) {
                int exitcode = WEXITSTATUS(wstatus);
                if (exitcode == -1) return -1;
            }
        }
    }
    return 0;
}
