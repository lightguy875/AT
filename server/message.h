#pragma once

#include "util.h"
#define MAX_STRING_SIZE 1000

/*!
 * \brief Message.
 *
 * The structure of the message that will be sent between client and server
 */
typedef struct msgbuf {
  ///> The type of the message
  long type;

  ///> The time in seconds since 70s
  int t;

  ///> The path to the executables
  char s[MAX_STRING_SIZE];
} Msg;

/*!
 *  \brief Prints the content of message.
 *  \param msg a message.
 *  \return nothing.
 */
void msg_print(Msg*);