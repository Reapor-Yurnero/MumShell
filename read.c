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

// the read process of input command
// handles the uncompleted quote
int getCommand(char* commandline)
{
    char buffer[1024];
    unsigned int buffer_id = 0;
    char c;
    bool singlequoted = false, doublequoted = false;
    while (true) {
        read(STDOUT_FILENO, &c, 1);
        if ((int)c == 4) return -1; // end of transmission
        else if ((int)c == 3) return -2; // end of context
        else if ((int)c == 127) // backspace
        {
            if (buffer_id != 0) buffer_id--;
            continue;
        }
        else buffer[buffer_id++] = c;
        // detect single/double quotes
        singlequoted ^= !doublequoted && (c == '\'');
        doublequoted ^= !singlequoted && (c == '\"');
        if (c == '\n')
        {
            if (singlequoted || doublequoted )
            {
                printf("\r\33[K> ");
                fflush(stdout);
                buffer_id--;
            }
            else break;
        }
    }
    strcpy(commandline, buffer);
    // test for read result
    for (unsigned int i = 0; i< buffer_id;++i) printf("%c",commandline[i]);
    fflush(stdout);
    return 0;
}
