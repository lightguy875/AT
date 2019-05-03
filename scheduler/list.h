#pragma once

#include "util.h"

typedef struct node Node;

typedef struct list List;

void list_push_back(List*, void*);

void* list_pop_back(List*);

Node* node_create(void*);

List* list_create();

void list_destroy(List*);