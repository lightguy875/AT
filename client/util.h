#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <time.h>

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

#define W(x) printf("%s%s = %s%s\n", RED, #x, x, RESET);

bool try_cast_int (char*, int*);