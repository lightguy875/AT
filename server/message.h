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

  ///> The delay in seconds
  int delay;

  ///> The path to the executables
  char s[MAX_STRING_SIZE];
} Msg;

/*!
 *  \brief Prints the content of message.
 *  \param msg a message.
 */
void msg_print(Msg* msg);

/*!
 *  \brief Create a queue.
 *  \param key the key to que queue.
 *  \return The queue id.
 */
int queue_create(int key);

/*!
 *  \brief Prints the content of message.
 *  \param key the key to que queue.
 *  \return The queue id.
 */
int queue_retrieve(int key);