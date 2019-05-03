#include "util.h"

#include "fat_tree.h"
#include "hypercube.h"
#include "torus.h"

#include "job.h"

// Data
int topology_type;
List *jobs;

void create_process_vector (int v[N]) {
  for (int i = 0; i < N; i++) {
    v[i] = fork();

    if (!v[i]) {
      break;
    }
  }
}

void create_process_matrix (int v[M][M]) {
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < M; j++) {
      v[i][j] = fork();

      if (!v[i][j]) {
        break;
      }
    }
  }
}

void create_process () {
  switch (topology_type) {
    case TREE:
      create_process_vector(ft);
      break;
    case HYPERCUBE:
      create_process_vector(hc);
      break;
    case TORUS:
      create_process_matrix(tr);
      break;
    default:
      E("Wrong topology!");
      exit(1);
  }
}


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
