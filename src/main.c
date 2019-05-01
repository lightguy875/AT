#include <stdio.h>

typedef struct {
  bool shouldDoThis;
  char* filename;
} State;

void make_state (int sz, char *flags[], State* st) {
  st->shouldDoSomething = false;
  st.filename = "";

  for (int i = 1; i < sz; i++) {
    if (strcmp(flags[i], "-f")) {
      st->shouldDoSomenthing = true;
      
      continue;
    }
    
    // check if it is "--filename"
    if (strlen(flags[i]) > 2 && strlen(flags[i]) < 103 && flags[i][0] == "-" && flags[i][1] == "-") {
      // save the pointer to the name of the file
      str->filename = flags[i];
      
      // remove the "--"
      str->filename += 2;
      
      continue;
    }
  }
}

int main (int argc, char *argv[]) {
  State st;
  
  make_state(&st);
  
  // Do stuff base on state
  
  return 0;
}
