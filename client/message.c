#include "message.h"

void msg_print(Msg* msg) {
    printf("Message Content:\n\tType: %ld\n\tSeconds: %d\n\tMessage: %s\n", msg->type, msg->t, msg->s);
}