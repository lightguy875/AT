#include "util.h"


void send (int seconds, char* filename) {
  // Send a message to the scheduler
} 

int main (int argc, char *argv[]) {
  int seconds = 0;
  char *filename = NULL;

  if (argc == 3 && try_cast_int(argv[1], &seconds) && argv[2] != NULL) {
    filename = argv[2];

    send(seconds, filename);
  }
  
  return 0;
}
