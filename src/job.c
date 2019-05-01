#include "job.h"
#include "list.h"

struct job {
    int id;
    int seconds;
    char* filename;
};

Job* job_create(int seconds, char* filename) {
    Job *job = (Job*)malloc(sizeof(Job));

    job->id = ++nxt;
    job->seconds = seconds;
    job->filename = filename;

    return job;
}