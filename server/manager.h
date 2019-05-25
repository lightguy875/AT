#include "util.h"

#define PATH "../prog/"
#define PATH_SIZE 50
#define JOB_FINISHED 1
#define NEW_JOB 2

int topology_type;

bool if_worker(int);

time_t get_time();

void to_manage(int, int);