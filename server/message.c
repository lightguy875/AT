#include "message.h"

void msg_print(Msg* msg) {
    printf("%sMessage Content:\n\tType: %ld\n\tSeconds: %d (Since 70's)\n\tMessage: %s\n%s", GREEN, msg->type, msg->t, msg->s, RESET);
}

int queue_create(int key) {
    int queue_id = msgget(key, IPC_CREAT); // TODO: check if I should send other flag

    if (queue_id < 0) {
        E("Failed to get queue");
    }

    return queue_id;
}

int queue_retrieve(int key) {
	int queue_id = msgget(key, MSG_FLAG);

	if (queue_id < 0) {
		E("Failed to get queue");
	}

    return queue_id;
}