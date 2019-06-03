#include "message.h"

void msg_print(Msg* msg) {
    printf("%sMessage Content:\n\tType: %ld\n\tSeconds: %d (Since 70's)\n\tDelay: %ds\n\tMessage: %s\n%s", YELLOW, msg->type, msg->t, msg->delay, msg->s, RESET);
}

int queue_create(int key) {
    int queue_id = msgget(key, IPC_CREAT | 0777);

    if (queue_id < 0) {
        E("Failed to get queue");
    }

    return queue_id;
}

int queue_retrieve(int key) {
	int queue_id = msgget(key, MSG_FLAG | 0777);

	if (queue_id < 0) {
		E("Failed to get queue");
	}

    return queue_id;
}