#include "util.h"

#include "job.h"
#include "list.h"
#include "message.h"

#include "fat_tree.h"
#include "hypercube.h"
#include "torus.h"

// Data
int pid;

int topology_type;

// int structure[N];

void create_process () {
  for (int i = 0; i < N; i++) {
    if (!(pid = fork())) {
      break;
    }
  }
}

void task () {
  switch (topology_type) {
    case TREE:
      // ft_task(structure);
      break;
    case HYPERCUBE:
      // hc_task(structure);
      break;
    case TORUS:
      // tr_task(structure);
      break;
    default:
      E("Wrong topology!");
      exit(1);
  }
}

void on_receive_message (List* jobs, int seconds, char* filename) {
  Job *job = job_create(seconds, filename);

  list_push_back(jobs, job);
}

void get_topology (int n, char *v[]) {
  if (n == 2 && try_cast_int(v[1], &topology_type)) {
    S("Topology set");
  } else {
    E("Not a valid topology");
    exit(1);
  }
}

Job* next_job (List* jobs) {
  time_t t = time(NULL);
  Job *job = NULL;
  Node *curr = jobs->begin;

  while (curr != NULL) {
      Job *aux = (Job*) curr->value;

      if (job == NULL || job->seconds > aux->seconds) {
          job = aux;
      }

      curr = curr->nxt;
  }

  return (job != NULL && job->seconds <= t) ? job : NULL;
}

//TODO: add errno to improve error handling
void schedule () {
  int key = KEY;
  int id = msgget(key, IPC_CREAT); // TODO: check if I should send other flag

  if (id < 0) {
    E("Failed to get queue");
  } 

  Msg msg;

  while (true) {
    int res = msgrcv(id, &msg,  sizeof(Msg) /* - sizeof(long) */, N, 0);

    if (res < 0) {
      E("Failed to receive message");
    } else {
      S("Message received");
      msg_print(&msg);
    }
  }
}

int main (int argc, char *argv[]) {
  get_topology(argc, argv);
  create_process();

  if (pid != 0) { // is the scheduler
    List *jobs = list_create();

    schedule();

    list_destroy(jobs);
  }
  
  return 0;
}
