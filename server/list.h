#pragma once

#include "util.h"
#include "job.h"

typedef struct node {
    struct node* prev;

    struct node* nxt;

    void* value;
} Node;

typedef struct list {
    int sz;

    struct node *begin;

    struct node *end;
} List;

void list_push_back(List*, void*);

void* list_pop_back(List*);

Node* node_create(void*);

List* list_create();

void list_destroy(List*);

Job* list_next_job(List*);