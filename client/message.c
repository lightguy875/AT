#include <stdio.h>
#include "message.h"

void msg_print(Msg* msg) {
    printf("Message Content:\n\tType: %ld\n\tSeconds: %d (Since 70's)\n\tDelay: %ds\tMessage: %s\n", msg->type, msg->t, msg->delay, msg->s);
}