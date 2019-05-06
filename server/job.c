#include "job.h"

Job* job_create(int seconds, char* filename) {
    Job *job = (Job*)malloc(sizeof(Job));

    job->id = ++nxt;
    job->seconds = seconds;
    job->filename = filename;

    return job;
}