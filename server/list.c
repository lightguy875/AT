#include "list.h"

Node* node_create(void* value) {
    Node *node = (Node*) malloc(sizeof(Node));

    node->prev = NULL;
    node->nxt = NULL;
    node->value = value;

    return node;
}

List* list_create() {
    List *list = (List*) malloc(sizeof(List));

    list->sz = 0;
    list->begin = NULL;
    list->end = NULL;

    return list;
}

void list_push_back(List* list, void* value) {
    Node *new = node_create(value); 

    if (list->sz) {
        Node *last = list->end;

        last->nxt = new;
        new->prev = last;

        list->end = new;
    } else {
        list->begin = list->end = new;
    }

    list->sz++;
}

void* list_pop_back(List* list) {
    if (list->sz) {
        list->sz--;

        Node *last = list->end;
        Node *penultimate = last->prev;

        list->end = penultimate;
        last->prev = NULL;

        if (penultimate == NULL) {
            list->begin = NULL;
        } else {
            penultimate->nxt = NULL;
        }

        void* value = last->value;

        free(last);

        return value;
    }

    return NULL;
}

void list_destroy(List* list) {
    Node *curr = list->begin;

    while (curr != NULL) {
        Node *aux = curr->nxt;

        free(curr);

        curr = aux;
    }

    free(list);
    list = NULL;
}

Job* list_next_job (List* list) {
    Job *job = NULL;
    Node *curr = list->begin;

    while (curr != NULL) {
        Job *aux = (Job*) curr->value;

        if (job == NULL || job->seconds > aux->seconds) {
            job = aux;
        }

        curr = curr->nxt;
    }

    return job;
}