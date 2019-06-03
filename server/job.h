#pragma once
/**
 * @file job.h
 * @author Luís Eduardo Luz Sila - 15/0137885
 * @author Claudio Segala Rodrigues Filho 15/0032552
 * @author Yan Victor dos Santos 14/0033599
 * @brief 
 * @version 0.1
 * @date 2019-06-03
 * 
 * @copyright Copyright (c) 2019
 * 
 */

#include "util.h"
/**
 * @brief Um job com toda a estrutura que compõe o job
 * 
 */
typedef struct job {
    int id;
    int seconds;
    int delay;
    char filename[1000];
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

Job* job_create(int id, int seconds, char* filename, int delay);