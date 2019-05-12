#pragma once

#include "util.h"

typedef struct msgbuf {
  long type;
  int t;
  char s[MAX_STRING_SIZE];
} Msg;

void msg_print(Msg*);