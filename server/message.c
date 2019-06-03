#include "util.h"
#include "message.h"

void msg_print(Msg* msg) {
    printf("%sMessage Content:\n\tID: %ld\n\tType: %ld\n\tSeconds: %d (Since 70's)\n\tDelay: %ds\n\tMessage: %s\n%s", YELLOW, msg->id, msg->type, msg->t, msg->delay, msg->s, RESET);
}