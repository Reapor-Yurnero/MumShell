//
// Created by Reapor Yurnero on 03/10/2018.
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
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
        const process_t * p = &jobs->process[i];
        char* command = NULL;
        if (p->argc > 0) command = p->argv[0];
        else continue; // we ensure that valid (argc>=1) process will be executed
        if (strcmp(command, "exit") == 0) return -100;
        if (strcmp(command, "cd") == 0) {
            if (p->argc < 2) continue;
            else if (chdir(p->argv[1]) < 0) {
                //TODO: improve the error
                perror("cd: invalid path");
            }
            continue;
        }
        int fd[2];
        pipe(fd);
        out = (i == jobs->p_num - 1) ? STDOUT_FILENO : fd[1];
        // printf("executing process[%d]\n", i);
        int rtn = executeprocess(p, in, out, fd);
        in = fd[0];
        close(fd[1]); // ???
        //close(fd[0]);
        if (rtn != 0) return rtn;
    }
    return 0;
}

int executeprocess(const process_t* p, int in, int out, int* fd) {
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
        if (in != STDIN_FILENO) {
            close(STDIN_FILENO);
            dup2(in, STDIN_FILENO);
            close(in);
            // printf("%s stdin dup connected in=%d\n",p->argv[0],in);
        }
        if (out == STDOUT_FILENO) close(fd[1]);// printf("%d %d\n",fd[0],fd[1]);
        else { // out == fd[1]
            // printf("%s out=%d\n",p->argv[0], out);
            close(STDOUT_FILENO);
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
                perror("read input file failed.");
                //return -1;
            }
            else dup2(ifile, STDIN_FILENO);
        }
        if (p->outMode == FILEOUT) {
            ofile = open(p->outFile, O_WRONLY | O_CREAT | O_TRUNC);
            if (ofile < 0) {
                perror("write output file failed.");
                //return -1;
            }
            else dup2(ofile, STDOUT_FILENO);
        }
        else if (p->outMode == FILEAPPEND) {
            ofile = open(p->outFile, O_WRONLY | O_CREAT | O_APPEND);
            if (ofile < 0) {
                perror("append output file failed.");
                //return -1;
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
                perror("failed to execute");
                exit(rtn);
            }
        }
        if (p->outMode == FILEOUT || p->outMode == FILEAPPEND)
            close(ofile);
        if (p->inMode == FILEIN) close(ifile);
        // end the child process anyway
        // printf("childprocess end\n");
        exit(0);
    }
    else { // parent process
        if (out == STDOUT_FILENO) {
            // wait all child process to terminate at the end of jobs
            int wstatus;
            // printf("parent waiting..\n");
            while (waitpid(-1,&wstatus,0) != -1);
            // printf("parent ends waiting\n");
        }
        // else printf("parent nothing to do\n");
    }
    return 0;
}
