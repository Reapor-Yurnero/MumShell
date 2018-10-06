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

static int sigint = 0;

static void sigHandler(int sig_num) {
    (void)sig_num;
    sigint = 1;
}

int main()
{
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = sigHandler;
    sa.sa_flags = 0;// not SA_RESTART!;

    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);

    while (1)
    {
        char inputcommand[MAX_CL_LEN] = {0};
        if (sigint) {fflush(stdout);printf("\n");fflush(stdout);sigint=0;}
        printf("mumsh $ ");
        fflush(stdout);
        int len = getCommand(inputcommand);
        //printf("%d\n",len);
        if (len == -1) {printf("exit\n");exit(0);}
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
        if (rtn == -100) {printf("exit\n");exit(0);}
        //else if (rtn < 0) continue;
        freejobs(&jobs);
        //break;
    }
    //return 0;
}
