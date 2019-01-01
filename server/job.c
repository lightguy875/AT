#include "util.h"
#include "job.h"

Job* job_create(int id, int seconds, char* filename, int delay) {
    Job *job = (Job*)malloc(sizeof(Job));

    job->id = id;
    job->seconds = seconds;
    job->delay = delay;
    job->done = false;
    job->submission = time(NULL);
    job->start = 0;
    job->finish = 0;
    job->completed = 0;

    strcpy(job->filename, filename);

    for (int i = 0; i < N; i++) {
        job->node_time[i] = -1;
        job->node_pid[i] = -1;
    }

    return job;
}