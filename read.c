//
// Created by Reapor Yurnero on 03/10/2018.
//
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include "read.h"
#include "para.h"

// the read process of input command
// handles the uncompleted quote
int getCommand(char* commandline)
{
    char buffer[MAX_CL_LEN] = {0};
    unsigned int buffer_id = 0;
    int c;
    bool singlequoted = false, doublequoted = false,
    pipe_found = true, redirect_found = true;
    while (true) {
        //ssize_t readrtn = read(STDOUT_FILENO, &c, 1);
        c = getchar();
        if ((int)c == EOF) {
            // ctrl-d
            if (feof(stdin)) {
                if (buffer_id == 0) {
                    //printf("\n");
                    return -1;
                }
                else continue;
            }
            // ctrl-c
            else if(ferror(stdin)) return -2;
//            fflush(stdout);
            //printf("EOF\n");
//            if (buffer_id == 0) {
//                printf("\n");
//                return -1;
//            }
//            else continue;
        } // end of transmission
        else if ((int)c == 3) {printf("ctrl-c\n");return -2;}// end of context
        else if ((int)c == 127) // backspace
        {
            if (buffer_id != 0) buffer_id--;
            continue;
        }
        else buffer[buffer_id++] = (char)c;
        // detect single/double quotes
        singlequoted ^= !doublequoted && (c == '\'');
        doublequoted ^= !singlequoted && (c == '\"');
        if (c == '>' || c == '<') {
            redirect_found = false;
        }
        else if (!redirect_found) {
            if (c != ' ' && c!= '\n') redirect_found = true;
        }
        if (c == '|') {
            pipe_found = false;
        }
        else if (!pipe_found && c!= ' ' && c!='\n') pipe_found = true;
        if (c == '\n')
        {
            if (singlequoted || doublequoted || !pipe_found || !redirect_found)
            {
                printf("\r\33[K> ");
                fflush(stdout);
                buffer_id--;
            }
            else {buffer_id--; break;}
        }
    }
    strcpy(commandline, buffer);
    // test for read result
//    for (unsigned int i = 0; i< buffer_id;++i) printf("%c",commandline[i]);
//    printf("\n");
    fflush(stdout);
    return buffer_id;
}
