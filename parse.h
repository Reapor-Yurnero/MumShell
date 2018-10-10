//
// Created by Reapor Yurnero on 03/10/2018.
//
#ifndef PROJECT_PARSE_H
#define PROJECT_PARSE_H

#include "para.h"

enum IOMode {STDIN, STDOUT, FILEIN, FILEOUT, FILEAPPEND};

typedef struct process_t process_t;
struct process_t {
    int argc;
    char* argv[MAX_ARG_NUM];
    char* inFile;
    char* outFile;
    //char* bak;
    //int null_id;
    enum IOMode inMode;
    enum IOMode outMode;
};

typedef struct jobs_t jobs_t;
struct jobs_t {
    int p_num;
    process_t process[MAX_PIPE_NUM];
};

void initprocess(process_t* p);
int parseCommandLine(char* inputCommand, int c_len, jobs_t* jobs);
//int parsePipe()
int parseProcess(char* inputCommand, int c_start, int c_end, process_t* p);
void displayjobs(const jobs_t* jobs);
void displayprocess(const process_t* p);
void freejobs(jobs_t* jobs);
void freeprocess(process_t* p);

#endif //PROJECT_PARSE_H
