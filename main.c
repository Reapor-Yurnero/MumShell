// C program to demonstrate use of fork() and pipe() 
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<sys/types.h>
#include<string.h>
#include<sys/wait.h>
#include "read.h"

int main()
{
    while (1)
    {
        char inputcommand[1024];
        printf("mumsh $ ");
        fflush(stdout);
        getCommand(inputcommand);
        break;
    }
}
