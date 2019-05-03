#include "util.h"

#include "fat_tree.h"
#include "hypercube.h"
#include "torus.h"

#include "job.h"

// Data
int topology_type;

List *jobs;

int structure[N];

void create_process () {
  for (int i = 0; i < N; i++) {
    structure[i] = fork();

    if (!structure[i]) {
      break;
    }
  }
}

// void task () {
//   switch (topology_type) {
//     case TREE:
//       ft_task(structure);
//       break;
//     case HYPERCUBE:
//       hc_task(structure);
//       break;
//     case TORUS:
//       tr_task(structure);
//       break;
//     default:
//       E("Wrong topology!");
//       exit(1);
//   }
// }

void on_receive_message (int seconds, char* filename) {
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

void destroy_list () {
  list_destroy(jobs);
}

void create_list () {
  jobs = list_create();
  S("List created");
}

int main (int argc, char *argv[]) {
  get_topology(argc, argv);
  create_list();
  create_process();

  // Do something
  
  destroy_list();
  
  return 0;
}
