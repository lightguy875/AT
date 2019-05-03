#include "util.h"

#include "fat_tree.h"
#include "hypercube.h"
#include "torus.h"

#include "job.h"

// Data
List *jobs;


void on_receive_message (int seconds, char* filename) {
  Job *job = job_create(seconds, filename);

  list_push_back(jobs, job);
}

int main (int argc, char *argv[]) {
  int seconds = 0;

  jobs = list_create();

  // Do something
  
  list_destroy(jobs);
  
  return 0;
}
