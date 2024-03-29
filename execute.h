//
// Created by Reapor Yurnero on 03/10/2018.
//

#ifndef PROJECT_EXECUTE_H
#define PROJECT_EXECUTE_H

#include "parse.h"
#include "para.h"
#include <stdbool.h>

int executejobs(jobs_t* jobs);

int executeprocess(process_t* p, int in, int out, int* fd, jobs_t* jobs);

void reapechildren(jobs_t* jobs);

void showjobslist(jobs_t* jobslist, int size);

int verifyjobs(jobs_t* jobs);

#endif //PROJECT_EXECUTE_H
