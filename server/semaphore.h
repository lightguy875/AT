#pragma once

#include "util.h"

#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

typedef struct semaphore Semaphore;

/*!
 *  \brief Get the index of the father of the node at the given index.
 *  \param sem the semaphore.
 *  \param idx the index.
 *  \param num the number of operations.
 *  \param op the operation.
 *  \param flag the flag.
 */
void sem_op (Semaphore* sem, int idx, int num, int op, int flag);

/*!
 *  \brief Increase the semaphore
 *  \param sem the semaphore.
 */
void P(Semaphore* sem);

/*!
 *  \brief Decrease the semaphore
 *  \param sem the semaphore.
 */
void V(Semaphore* sem);