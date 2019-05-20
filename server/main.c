#include "util.h"

#include "job.h"
#include "list.h"
#include "message.h"
#include "manager.h"

#include "fat_tree.h"
#include "hypercube.h"
#include "torus.h"

// Data
int topology_type;
int id_message;

int structure[N+1];
int vis[N+1];

void create_managers () {
  int idx = create_manager();
  if (idx >= 0) 
    to_manage(idx, TREE);

  switch (topology_type) {
    case TREE:
    case 0:
      ft_make(structure, vis);
      break;
    case HYPERCUBE:
      hc_make(structure, vis);
      break;
    case TORUS:
      tr_make(structure, vis);
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

void exec(char *filename, int id) {
  Msg msg = { 0, 0 };
  strcpy(msg.s, filename);
  msgsnd(id, &msg,  sizeof(Msg), 0);
}

void run (List* jobs, int id) {
  Job *job = next_job(jobs);

  // TODO: send the processes execute the file
  exec(job->filename, id);
}

//TODO: add errno to improve error handling
void schedule (List* jobs) {
  int key = KEY;
  id_message = msgget(key, IPC_CREAT); // TODO: check if I should send other flag

  if (id_message < 0) {
    E("Failed to get queue");
  } 

  Msg msg;
  create_managers();

  while (true) {
    int res = msgrcv(id_message, &msg,  sizeof(Msg) /* - sizeof(long) */, N+1, 0);

    if (res < 0) {
      // Failed to receive a message for some reason

      if (errno == EINTR && alarm(0) == 0) {  // https://stackoverflow.com/questions/41474299/checking-if-errno-eintr-what-does-it-mean
        // Checked if the interruption error was caused 
        // by an alarm (deactivates the alarm in the process)

        run(jobs, id_message);
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

          run(jobs, id_message);
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

void destroy(List *jobs) {
  struct msqid_ds msg;
  list_destroy(jobs);
  msgctl(id_message, IPC_RMID, &msg);
}

int main (int argc, char *argv[]) {
  setup();
  get_topology(argc, argv);

  List *jobs = list_create();

  schedule(jobs);
  destroy(jobs);
  
  return 0;
}
