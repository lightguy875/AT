#include "util.h"


void send (int t, char* filename) {
  // Send a message to the scheduler
} 

int main (int argc, char *argv[]) {
  int seconds = 0;

  if (argc == 3 && try_cast_int(argv[1], &seconds) && argv[2] != NULL) {
    char *filename = argv[2];
    time_t t = time(NULL) + seconds;

    send(t, filename);
  }
  
  return 0;
}
