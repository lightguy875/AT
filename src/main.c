#include "util.h"

#include "fat_tree.h"
#include "hypercube.h"
#include "torus.h"

typedef struct {
  bool shouldDoThis;
  char* filename;
} State;

void make_state (int sz, char *flags[], State* st) {
  st->shouldDoThis = false;
  st->filename = "";

  for (int i = 1; i < sz; i++) {
    if (strcmp(flags[i], "-f")) {
      st->shouldDoThis = true;
      
      continue;
    }
    
    // check if it is "--filename"
    if (strlen(flags[i]) > 2 && strlen(flags[i]) < 103 && flags[i][0] == '-' && flags[i][1] == '-') {
      // save the pointer to the name of the file
      st->filename = flags[i];
      
      // remove the "--"
      st->filename += 2;
      
      continue;
    }
  }
}

int main (int argc, char *argv[]) {
  State st;
  
  make_state(argc, argv, &st);
  
  // Do stuff base on state
  
  return 0;
}
