// C program to demonstrate use of fork() and pipe() 
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <string.h>
#include <signal.h>
#include <sys/wait.h>
#include "read.h"
#include "para.h"
#include "parse.h"
#include "execute.h"

void handler(int sig_num) {
    (void)sig_num;
}

int main()
{
    signal(SIGINT,handler);
    while (1)
    {
        char inputcommand[MAX_CL_LEN];
        printf("mumsh $ ");
        fflush(stdout);
        int len = getCommand(inputcommand);
        //printf("%d\n",len);
        if (len == -1) return 0;
        else if (len <= 0) continue;
        jobs_t jobs;
        int rtn = parseCommandLine(inputcommand,len,&jobs);
        if (rtn < 0) {
            freejobs(&jobs);
            continue;
        }
        // displayjobs(&jobs);
        // printf("=======================\n");
        rtn = executejobs(&jobs);
        if (rtn == -100) exit(0);
        //else if (rtn < 0) continue;
        freejobs(&jobs);
        //break;
    }
}
