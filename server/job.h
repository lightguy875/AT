#pragma once

#include "util.h"

/*!
 * \brief A Job.
 *
 * An struct that holds the informations of job.
 */
typedef struct job {
    int id;
    int seconds;
    int makespan;
    int delay;
    char* filename;
    bool done;
    char* report;
} Job;

/*!
 * \brief A counter for ids.
 *
 * Static counter to give the jobs an unique id.
 */
static int nxt;

/*!
 *  \brief Create a job.
 *  \param seconds the time in which should be executed.
 *  \param filename the file path to the program.
 *  \return a pointer to the node
 */
Job* job_create(int seconds, char* filename, int delay);