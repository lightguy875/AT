#include "util.h"
#include "message.h"

void send (int t, char filename[]) {
  int key = KEY; // matricula truncada
  int id = msgget(key, 0); // TODO: check if I should sen other flag

  if (id < 0) {
    E("Failed to get queue");
  }

  Msg msg = { N+1, t };
  strcpy(msg.s, filename);
  int res = msgsnd(id, &msg, sizeof(Msg) /* - sizeof(long) */, 0); // TODO: check if I should sen other flag

  if (res < 0) {
    E("Failed to send message");
  } else {
    S("Message sent");
    msg_print(&msg);
  }
} 

void destroy() {
	struct msqid_ds msg;
  msgctl(65536, IPC_RMID, &msg);
}

int main (int argc, char *argv[]) {
  int seconds = 0;
  destroy();
  // if (argc == 3 && try_cast_int(argv[1], &seconds) && argv[2] != NULL) {
  //   char filename[MAX_STRING_SIZE];
  //   time_t t = time(NULL) + seconds;

  //   strcpy(filename, argv[2]);

  //   send(t, filename);
  // }
  
  return 0;
}
