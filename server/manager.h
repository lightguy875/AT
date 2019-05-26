#include "util.h"
#include "message.h"

#include "tree.h"
#include "hypercube.h"
#include "torus.h"

#define PATH "../prog/"
#define PATH_SIZE 50
#define JOB_FINISHED 1
#define NEW_JOB 2

int topology_type;

Msg execute_job(int, char*);

void send_msg_ft(int, Msg, int);

void broadcast(int, Msg, int);

void receive_msg(int);

void to_manage(int, int);