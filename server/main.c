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

void run (List* jobs) {
  Job *job = next_job(jobs);

  // TODO: send the processes execute the file
  //exec(job->filename);
}

//TODO: add errno to improve error handling
void schedule (List* jobs) {
  int key = KEY;
  int id = msgget(key, IPC_CREAT); // TODO: check if I should send other flag

  if (id < 0) {
    E("Failed to get queue");
  } 

  Msg msg;

  while (true) {
    int res = msgrcv(id, &msg,  sizeof(Msg) /* - sizeof(long) */, N, 0);

    if (res < 0) {
      // Failed to receive a message for some reason

      if (errno == EINTR && alarm(0) == 0) {  // https://stackoverflow.com/questions/41474299/checking-if-errno-eintr-what-does-it-mean
        // Checked if the interruption error was caused 
        // by an alarm (deactivates the alarm in the process)

        run(jobs);
      } else {
        // The interruption error was not caused by the alarm or 
        // failed to receive (no interruption error)

        E("Failed to receive message");
      }
    } else {
      // Receive the message succeffuly

      S("Message received");

      msg_print(&msg);
      
      Job *job = job_create(msg.t, msg.s);
      Job *nxt_job = next_job(jobs);

      list_push_back(jobs, job);

      if (nxt_job == NULL || nxt_job->seconds > job->seconds) {
        // Checking if we should adjust the alarm for the next job
        // If there was no next job, create an alarm for it, otherwise
        // we will be modifying it to reduce the alarm time

        time_t now = time(NULL);

        if (job->seconds - now <= 0) {
          // Deactivate the alarm and execute if it was
          // to execute now the file

          alarm(0);

          run(jobs);
        } else {
          // Modify the alarm for the new job, since it is 
          // closer to execute

          alarm(job->seconds - now);
        }
      } 

      list_push_back(jobs, job);
    }
  }
}

void setup () {
  signal(SIGALRM, *dummy);
}

int main (int argc, char *argv[]) {
  setup();
  get_topology(argc, argv);
  create_process();

  if (pid != 0) { // is the scheduler
    List *jobs = list_create();

    schedule(jobs);

    list_destroy(jobs);
  }
  
  return 0;
}
