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
    int buffer_id = 0;
    int c;
    bool singlequoted = false, doublequoted = false,
    pipe_found = true, redirect_found = true,
    inred_error = false, outred_error = false,
    pipesymbol_error = false, pipe_noprogram = false;
    int outred_id = -1, pipesym_id = -1;
    while (true) {
        //ssize_t readrtn = read(STDOUT_FILENO, &c, 1);
        c = getchar();
        if (c == EOF) {
            // ctrl-d
            if (feof(stdin)) {
                if (buffer_id == 0) {
                    //printf("\n");
                    return -1; //exit the shell
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
        else if (c == 3) {printf("ctrl-c\n");return -2;}// end of context
        else if (c == 127) // backspace
        {
            if (buffer_id != 0) buffer_id--;
            continue;
        }
        else buffer[buffer_id++] = (char)c;
        // detect single/double quotes
        singlequoted ^= !doublequoted && (c == '\'');
        doublequoted ^= !singlequoted && (c == '\"');
        // detect > <
        if (c == '>') {
            // not the first time of > and adjacent to last >
            // escape the case of >>
            if (outred_id != -1 && buffer_id-1 == outred_id+1) {
                redirect_found = false;
                outred_id = buffer_id -1;
            }
            // any other case of >
            else {
                if (!redirect_found) outred_error = true;
                outred_id = buffer_id - 1;
                redirect_found = false;
            }
        }
        else if (c == '<') {
            if (!redirect_found) inred_error = true;
            redirect_found = false;
        }
        else if (!redirect_found) {
            if (c != ' ' && c!= '\n' && c!= '|') redirect_found = true;
        }
        // detect |
        if (c == '|') {
            if (!redirect_found) pipesymbol_error = true;
            if (pipesym_id != -1 && !pipe_found) {
                pipe_noprogram = true;
            }
            else {
                pipesym_id = buffer_id - 1;
                pipe_found = false;
            }
        }
        else if (!pipe_found && c!= ' ' && c!='\n') pipe_found = true;
        if (c == '\n')
        {
            // uncompleted quotes
            if (singlequoted || doublequoted)
            {
                printf("> ");
                fflush(stdout);
                //buffer_id--;
            }
            else if (inred_error) {
                printf("syntax error near unexpected token `<'\n");
                return -2;
            }
            else if (outred_error) {
                printf("syntax error near unexpected token `>'\n");
                return -2;
            }
            else if (pipesymbol_error) {
                printf("syntax error near unexpected token `|'\n");
                return -2;
            }
            else if (pipe_noprogram) {
                printf("error: missing program\n");
                return -2;
            }
            // uncompleted pipe and redirection
            else if (!pipe_found || !redirect_found) {
                printf("> ");
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
