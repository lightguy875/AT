#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include <time.h>
#include <unistd.h>

#include <sys/msg.h>
#include <sys/stat.h>

#include "message.h"

#define KEY 15003
#define N 15

#define S(x) printf("\033[0;32m%s\033[0m\n", x); // Output is green
#define E(x) printf("\033[0;31m%s\033[0m\n", x); // Output is red

/*!
 *  \brief Cast string to int.
 *  \param num a string with a number.
 *  \param result a pointer to the result of the convertion.
 *  \return if it succeeded or not.
 */
bool try_cast_int (char *num, int *result) {
    int sz = strlen(num);
    
    for (int i = sz - 1, n = 1; i >= 0; i--, n *= 10) {
        if (num[i] >= '0' && num[i] <= '9') {
            (*result) += n * (num[i] - '0');
        } else {
            return false;
        }
    }

    return true;
}

/*!
 *  \brief Send a message with a job to the queue.
 *  \param t the time in which the job should be executed.
 *  \param filename the path of the file to be executed.
 */
void send (int seconds, char filename[]) {
	int key = KEY; // matricula truncada
	int id = msgget(key, S_IWUSR);

	time_t t = time(NULL) + seconds;

	if (id < 0) {
		E("Failed to get queue");
	} else {
		S("Got the queue");
	}

	Msg msg = { N+1, rand(), t, seconds, N+1 };

	strcpy(msg.s, filename);
	int res = msgsnd(id, &msg, sizeof(Msg) - sizeof(long), 0);

	if (res < 0) {
		E("Failed to send messages");
	} else {
		S("Message sent");
		msg_print(&msg);
	}
} 

/*!
 *  \brief Destroy the queue
 */
void destroy() {
	int id = msgget(KEY, 0);

	printf("Destroying the queue %d\n", id);
	struct msqid_ds msg;
	msgctl(id, IPC_RMID, &msg);
}

int main (int argc, char *argv[]) {
	int seconds = 0;

	if (argc == 1) {
		destroy();
	}

	if (argc == 3 && try_cast_int(argv[1], &seconds) && argv[2] != NULL) {
		char filename[MAX_STRING_SIZE];

		strcpy(filename, argv[2]);

		send(seconds, filename);
	}

	return 0;
}
