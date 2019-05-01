#pragma once

#include "util.h"

#include <errno.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>

typedef struct semaphore Semaphore;

void sem_op (Semaphore*, int, int, int, int);

void P();

void V();