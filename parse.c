//
// Created by Reapor Yurnero on 03/10/2018.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include "parse.h"

void initprocess(process_t* p) {
    p->argc=0;
    p->inMode = STDIN;
    p->outMode = STDOUT;
    for (int i = 0; i < MAX_ARG_NUM;i++) p->argv[i]=(char*)malloc(MAX_ARG_LEN);
    p->inFile = (char*)malloc(64);
    //p->inFile = "nofile";
    p->outFile = (char*)malloc(64);
    //p->outFile = "nofile";
}

int parseCommandLine(char* inputCommand, int c_len, jobs_t* jobs)
{
    jobs->p_num = 0;
    char c;
    bool singlequoted = false, doublequoted = false, quoted;
    int c_start = 0, c_end = 0;
    for (int i = 0; i < c_len; ++i)
    {
        c = inputCommand[i];
        singlequoted ^= !doublequoted && (c == '\'');
        doublequoted ^= !singlequoted && (c == '\"');
        quoted = singlequoted || doublequoted;
        if (!quoted && c == '|')
        {
            if (i == 0) {
                perror("syntax error near unexpected token `|'");
                return -1;
            }
            else {
                c_start = (c_end ==0 )? c_end:c_end+1;
                c_end = i;
                initprocess(&jobs->process[jobs->p_num]);
                //printf("%d %d\n",c_start,c_end);
                int rtn = parseProcess(inputCommand,c_start,c_end,&jobs->process[jobs->p_num]);
                if (rtn < 0) return -1;
                jobs->p_num++;
            }
        }
        else if (!quoted && i == c_len-1) {
            c_start = (c_end ==0 )? c_end:c_end+1;
            c_end = i+1;
            initprocess(&jobs->process[jobs->p_num]);
            //printf("%d %d\n",c_start,c_end);
            int rtn = parseProcess(inputCommand,c_start,c_end,&jobs->process[jobs->p_num]);
            if (rtn < 0) return -1;
            jobs->p_num++;
        }
    }
    return 0;
}

int parseProcess(char* inputCommand, int c_start, int c_end, process_t* p)
{
    char c;
    bool singlequoted = false, doublequoted = false,
    quoted, ifilenotfound = false, ifiledes = false,
    ofilenotfound = false, ofiledes = false;
    int ifname_id = 0, ofname_id = 0, argv_id = 0;
    for (int i = c_start;i < c_end;++i) {
        c = inputCommand[i];
        singlequoted ^= !doublequoted && (c == '\'');
        doublequoted ^= !singlequoted && (c == '\"');
        quoted = singlequoted || doublequoted;
        if (!quoted)
        {
            if (c == ' ') {
                while (i != c_end-1 && inputCommand[i+1] ==' ') {
                    ++i;
                }
                if (!ifilenotfound && !ofilenotfound && i != c_end-1
                && p->argc!=0 && inputCommand[i+1] != ' '
                && inputCommand[i+1] != '<' && inputCommand[i+1] != '>')
                {
                    p->argv[p->argc-1][argv_id] = '\0';
                    ++p->argc;
                    argv_id=0;
                }
                ifiledes &= false; p->inFile[ifname_id] = '\0';
                ofiledes &= false; p->outFile[ofname_id] = '\0';
                if (i == c_end-1) {
                    //printf("endofprocessparse: argc = %d\n",p->argc);
                    p->argv[p->argc-1][argv_id] = '\0';
                }
                continue;
            }
            else if (c == '<') {
//                if (i == c_start) {
//                    perror("syntax error near unexpected token `newline'");
//                    return -1;
//                }
                p->inMode = FILEIN;
                ifilenotfound = true;
                ifiledes &= false; p->inFile[ifname_id] = '\0';
                ofiledes &= false; p->outFile[ofname_id] = '\0';
            }
            else if (c == '>') {
//                if (i == c_start) {
//                    perror("syntax error near unexpected token `newline'");
//                    return -1;
//                }
                if (i != c_end-1 && inputCommand[i+1] == '>')
                {p->outMode = FILEAPPEND;++i;}
                else p->outMode = FILEOUT;
                ofilenotfound = true;
                ifiledes &= false; p->inFile[ifname_id] = '\0';
                ofiledes &= false; p->outFile[ofname_id] = '\0';
            }
            else if (c == '\'' || c== '\"') {
                // TODO: handle quotes details
            }
            else {
                if (ifilenotfound) {ifilenotfound = false;ifiledes = true;}
                else if (ofilenotfound) {ofilenotfound = false;ofiledes = true;}
                if (ifiledes) {
                    p->inFile[ifname_id++] = c;
                }
                else if (ofiledes) {
                    p->outFile[ofname_id++] = c;
                }
                else {
                    if (p->argc == 0) {++p->argc;argv_id=0;}
                    //printf("argc:%d argv_id: %d %c\n", p->argc, argv_id, c);
                    p->argv[p->argc-1][argv_id++] = c;
                }
            }
            if (i == c_end-1) {
                //printf("endofprocessparse: argc = %d\n",p->argc);
                ifiledes &= false; p->inFile[ifname_id] = '\0';
                ofiledes &= false; p->outFile[ofname_id] = '\0';
                p->argv[p->argc-1][argv_id] = '\0';
                if (ifilenotfound || ofilenotfound) {
                    perror("syntax error near unexpected token `newline'");
                    return -1;
                }
            }
        }
        else { // quoted
            // TODO: consider quotes follow a redirect
            if (c == '\'' || c == '\"') {
                continue;
            }
            else {
                if (p->argc == 0) {++p->argc;argv_id=0;}
                p->argv[p->argc-1][argv_id++] = c;
            }
        }
    }
    p->argv[p->argc] = NULL;
    return 0;
}

void displayjobs(const jobs_t* jobs) {
    printf("displayjobs:\n");
    for (int i = 0;i < jobs->p_num;++i) {
        printf("process[%d]{\n",i);
        displayprocess(&jobs->process[i]);
        printf("}\n");
    }
}

void displayprocess(const process_t* p) {
    printf("argc: %d\n", p->argc);
    printf("argument: ");
    for (int i = 0;i < p->argc;++i) {
        printf("%s:",p->argv[i]);
    }
    printf("\n");
    if (p->inMode != 0)
        printf("inMode: %d inFile: %s\n",p->inMode, p->inFile);
    else printf("inMode: %d\n",p->inMode);
    if (p->outMode != 1)
        printf("outMode: %d outFile: %s\n",p->outMode, p->outFile);
    else printf("outMode: %d\n",p->outMode);
}

void freejobs(jobs_t* jobs) {
    for (int i = 0;i < jobs->p_num;++i) {
        freeprocess(&jobs->process[i]);
    }
}

void freeprocess(process_t* p) {
    for (int i = 0; i < MAX_ARG_NUM;i++) free(p->argv[i]);
    free(p->inFile);
    free(p->outFile);
}
