#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

#define RED "\033[0;31m"
#define GREEN "\033[0;32m"
#define YELLOW "\033[0;33m"
#define RESET "\033[0m"

#define MSG_FLAG 0x123

#define TREE 1
#define HYPERCUBE 2
#define TORUS 3

#define S(x) printf("%s%s%s\n", GREEN, x, RESET); fflush(stdout);
#define E(x) printf("%s%s%s\n", RED, x, RESET); fflush(stdout);

#define KEY 15003
#define N 15
#define M 4

#define PATH "../prog/"

/*!
 *  \brief Cast string to int.
 *  \param num a string with a number.
 *  \param result a pointer to the result of the convertion.
 *  \return if it succeeded or not.
 */
bool try_cast_int (char*, int*);

/*!
 *  \brief Does nothing.
 *  \return nothing.
 */
void dummy ();