#include "util.h"

int pid;

bool try_cast_int (char *num, int *result) {
    int sz = strlen(num);

    for (int i = sz - 1, n = 1; i >= 0; i--, n *= 10) {
        if (num[i] >= '0' && num[i] <= '9') {
            result += n * (num[i] - '0');
        } else {
            return false;
        }
    }

    return true;
}

int create_manager () {
  for (int i = 0; i < N; i++) {
    if (!(pid = fork())) 
      return i;
  }
  return -1;
}

int create_worker () {
    return fork();
}

int get_message_queue () {
    int key = KEY;
    return msgget(key, MSG_FLAG);
}

void dummy () {}
