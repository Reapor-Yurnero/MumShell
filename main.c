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

// function handler for sigint
static void sigHandler(int sig_num) {
    (void)sig_num;
    // set a flag which will trigger a new line
    sigint = 1;
}

int main()
{
    // parent process handle the SIGINT signal not to exit
    struct sigaction sa;
    memset(&sa, 0, sizeof(struct sigaction));
    sa.sa_handler = sigHandler;
    sa.sa_flags = 0;// not SA_RESTART! in this case, getchar will throw error and thus enter the next iteration in read
    sigaction(SIGINT, &sa, NULL);
    sigaction(SIGTERM, &sa, NULL);
    jobs_t bgjobslist[10]; int bgjobs_id = 0;
    while (1)
    {
        char inputcommand[MAX_CL_LEN] = {0};
        if (sigint) {fflush(stdout);printf("\n");fflush(stdout);sigint=0;}
        printf("mumsh $ ");
        fflush(stdout);
        bool background = false;
        int len = getCommand(inputcommand, &background);
        //printf("%s\n", inputcommand);
        //printf("%d\n",len);
        if (len == -1) {
            freejobslist(bgjobslist,bgjobs_id);
            printf("exit\n");
            exit(0);
        }
        else if (len <= 0) continue;
        //continue;
        jobs_t jobs; int rtn; jobs.p_num = 0;
        if (background) {
            if (bgjobs_id == 10) {
                printf("too many background programs\n");
                freejobslist(bgjobslist,bgjobs_id);
                freejobs(&jobs);
                printf("exit\n");
                exit(0);
            }
            bgjobslist[bgjobs_id].background = true;
            rtn = parseCommandLine(inputcommand,len,&bgjobslist[bgjobs_id]);
        }
        else {
            jobs.background = false;
            rtn = parseCommandLine(inputcommand,len,&jobs);
        }
        if (rtn < 0) {
            freejobs(&jobs);
            if (background) {
                freejobs(&bgjobslist[bgjobs_id]);
            }
            continue;
        }
        // displayjobs(&jobs);
        // printf("=======================\n");
        if (background) {
            printf("[%d] %s\n", bgjobs_id+1, bgjobslist[bgjobs_id].name);
            fflush(stdout);
            rtn = executejobs(&bgjobslist[bgjobs_id]);
            bgjobs_id++;
        }
        else rtn = executejobs(&jobs);
        if (rtn == -100) {
            freejobslist(bgjobslist,bgjobs_id);
            freejobs(&jobs);
            printf("exit\n");
            exit(0);
        }
        //else if (rtn < 0) continue;
        freejobs(&jobs);
        //break;
    }
    //return 0;
}
