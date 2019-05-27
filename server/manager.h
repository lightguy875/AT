#include "util.h"
#include "message.h"

// #include "tree.h"
// #include "hypercube.h"
// #include "torus.h"

#define PATH "../prog/"

int topology_type;

Msg execute_job(int, char*);

void send_msg_ft(int, Msg, int);

void broadcast(int, Msg, int);

void receive_msg(int);

void to_manage(int, int);