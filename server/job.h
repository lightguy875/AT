#pragma once

#include "util.h"

typedef struct job {
    int id;
    int seconds;
    char* filename;
} Job;

static int nxt;

Job* job_create(int, char*);