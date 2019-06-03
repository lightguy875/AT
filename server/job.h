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
    int delay;
    char* filename;
    bool done;
    int submission;
    int start;
    int finish;
    int completed;
    int node_time[N];
    int node_pid[N];
} Job;
/**
 * @brief Create a job
 * 
 * @param job id
 * @param seconds 
 * @param filename 
 * @param delay 
 * @return Job* a pointer to the node
 */
/*!
 *  \brief Create a job.
 *  \param seconds the time in which should be executed.
 *  \param filename the file path to the program.
 *  \return a pointer to the node
 */

Job* job_create(int id, int seconds, char* filename, int delay);