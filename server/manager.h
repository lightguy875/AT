#include "util.h"
#include "message.h"
#include "tree.h"

#define PATH "../prog/"
#define PATH_SIZE 50
#define JOB_FINISHED 1
#define NEW_JOB 2

int topology_type;

bool if_worker(int);

time_t get_time();

time_t job_time (time_t, time_t);

Msg execute_job(int, char*);

void send_msg_ft(int, Msg, int);

void send_msg_hc(int, Msg, int);

void send_msg_to(int, Msg, int);

void broadcast(int, Msg, int);

void receive_msg(int);

int get_message_queue();

void to_manage(int, int);