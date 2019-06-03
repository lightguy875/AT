#include "job.h"

Job* job_create(int id, int seconds, char* filename, int delay) {
    Job *job = (Job*)malloc(sizeof(Job));

    job->id = id;
    job->seconds = seconds;
    job->makespan = 0;
    job->delay = delay;
    job->filename = filename;
    job->done = false;
    job->report = (char *) malloc(1000);

    return job;
}