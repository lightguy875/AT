#include "util.h"

#include "fat_tree.h"
#include "hypercube.h"
#include "torus.h"

#include "job.h"

int main (int argc, char *argv[]) {
  int seconds = 0;

  List *jobs = list_create();

  if (argc == 3 && try_cast_int(argv[1], &seconds) && argv[2] != NULL) {
    Job *job = job_create(seconds, argv[2]);

    list_push_back(jobs, job);
  } 
  
  list_destroy(jobs);
  
  return 0;
}
