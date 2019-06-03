#pragma once

#define MAX_STRING_SIZE 1000

/*!
 * \brief Message.
 *
 * The structure of the message that will be sent between client and server
 */
typedef struct msgbuf {
  ///> The type of the message
  long type;

  ///> The job identifier
  long id;

  ///> The time in seconds since 70s
  int t;

  ///> The delay in seconds
  int delay;

  //> The id of the sender
  int origin;

  ///> The path to the executables
  char s[MAX_STRING_SIZE];
} Msg;
