#include "message.h"

void msg_print(Msg* msg) {
    printf("%sMessage Content:\n\tType: %ld\n\tSeconds: %d (Since 70's)\n\tMessage: %s\n%s", GREEN, msg->type, msg->t, msg->s, RESET);
}

int msg_create(int key) {
    int id = msgget(KEY, IPC_CREAT); // TODO: check if I should send other flag

    if (id < 0) {
        E("Failed to get queue");
    }

    return id;
}