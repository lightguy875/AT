#include "util.h"

typedef struct msgbuf {
  long type;
  int t;
  char s[MAX_STRING_SIZE];
} Msg;


void send (int t, char filename[]) {
  int key = KEY; // matricula truncada
  int id = msgget(key, 0); // TODO: check if I should sen other flag

  if (id < 0) {
    W(id);
    E("Failed to get queue");
  } 

  Msg msg = { N, t, *filename };

  int res = msgsnd(id, &msg, sizeof(Msg) /* - sizeof(long) */, 0); // TODO: check if I should sen other flag

  if (res < 0) {
    E("Failed to send message");
  } else {
    S("Message sent");
  }
} 

int main (int argc, char *argv[]) {
  int seconds = 0;

  if (argc == 3 && try_cast_int(argv[1], &seconds) && argv[2] != NULL) {
    char filename[MAX_STRING_SIZE];
    time_t t = time(NULL) + seconds;

    strcpy(filename, argv[2]);

    send(t, filename);
  }
  
  return 0;
}
