#include "job.h"

Job* job_create(int seconds, char* filename, int delay) {
    Job *job = (Job*)malloc(sizeof(Job));

    job->id = ++nxt;
    job->seconds = seconds;
    job->makespan = 0;
    job->delay = delay;
    job->filename = filename;
    job->done = false;
    job->report = (char *) malloc(1000);

    return job;
}