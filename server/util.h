#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define RED "\033[0;31m"
#define BOLDRED "\033[1;31m"
#define GREEN "\033[0;32m"
#define BOLDGREEN "\033[1;32m"
#define YELLOW "\033[0;33m"
#define BOLDYELLOW "\033[01;33m"
#define BLUE "\033[0;34m"
#define BOLDBLUE "\033[1;34m"
#define MAGENTA "\033[0;35m"
#define BOLDMAGENTA "\033[1;35m"
#define CYAN "\033[0;36m"
#define BOLDCYAN "\033[1;36m"
#define RESET "\033[0m"

#define TREE 1
#define HYPERCUBE 2
#define TORUS 3

#define W(x) printf("%s%s = %s%s\n", RED, #x, x, RESET);
#define S(x) printf("%s%s%s\n", GREEN, x, RESET);
#define L(x) printf("%s%s%s\n", YELLOW, x, RESET);
#define E(x) printf("%s%s%s\n", RED, x, RESET);

#define MAX_STRING_SIZE 1000
#define KEY 15003
#define N 16
#define M 4

/*!
 *  \brief Cast string to int.
 *  \param num a string with a number.
 *  \param result a pointer to the result of the convertion.
 *  \return if it succeeded or not
 */
bool try_cast_int (char*, int*);