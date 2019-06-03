#pragma once
/**
 * @file util.h
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
/**
 * @brief Converte char em string
 * 
 * @param string passa a string para ser transformada em inteiro
 * @param inteiro  passa o inteiro que receberá a string
 * @return retorna um booleano
 */
bool try_cast_int (char* string, int* inteiro);
