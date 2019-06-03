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
 *  \brief Create a job.
 *  \param seconds the time in which should be executed.
 *  \param filename the file path to the program.
 *  \return a pointer to the node
 */
Job* job_create(int id, int seconds, char* filename, int delay);