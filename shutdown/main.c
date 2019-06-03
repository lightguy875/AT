#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <time.h>
#include <unistd.h>

#include <errno.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <signal.h>

#include "message.h"

#define SCHEDULER 200
#define SHUTDOWN 201

#define KEY 15003
#define N 15

#define S(x) printf("\033[0;32m%s\033[0m\n", x); // Output is green
#define E(x) printf("\033[0;31m%s\033[0m\n", x); // Output is red

int main () {
	int key = KEY; // matricula truncada
	int queue_id = msgget(key, S_IWUSR);

	if (queue_id < 0) {
		E("Failed to get queue");
	} else {
		S("Got the queue");
	}

    int count = N + 1;

	while (count--) {
        Msg msg;

		int virtual_queue = SHUTDOWN;
		int res = msgrcv(queue_id, &msg, sizeof(Msg) - sizeof(long), virtual_queue, 0);
		
		if (res < 0) {
			E("Failed to receive message. Maybe the queue died?");
			break;
		}

		kill(msg.id, SIGTERM);
	}

	return 0;
}
