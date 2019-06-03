#pragma once
#include <time.h>

typedef struct Dado_processo
{
    pid_t pid;
    time_t inicio;
    time_t fim;
    time_t tempo_execucao;
    char * filename;
}Proess_info;