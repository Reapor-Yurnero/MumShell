// C program to demonstrate use of fork() and pipe() 
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>
#include "read.h"
#include "para.h"
#include "parse.h"

int main()
{
    while (1)
    {
        char inputcommand[MAX_CL_LEN];
        printf("mumsh $ ");
        fflush(stdout);
        int len = getCommand(inputcommand);
        printf("%d\n",len);
        if (len <= 0) continue;
        jobs_t jobs;
        parseCommandLine(inputcommand,len,&jobs);
        displayjobs(&jobs);
        freejobs(&jobs);
        //break;
    }
}
