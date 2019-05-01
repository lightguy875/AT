#include "util.h"
#include "list.h"

typedef struct job Job;

static int nxt;

Job* job_create(int, char*);