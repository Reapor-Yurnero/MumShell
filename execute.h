//
// Created by Reapor Yurnero on 03/10/2018.
//

#ifndef PROJECT_EXECUTE_H
#define PROJECT_EXECUTE_H

#include "parse.h"
#include "para.h"

int executejobs(jobs_t* jobs);

int executeprocess(const process_t* p, int in, int out, int* fd, int pnum);

#endif //PROJECT_EXECUTE_H
