#pragma once

#include "util.h"
#include "job.h"

/*!
 * \brief A node.
 *
 * The node of a doubly linked list
 */
typedef struct node {
    ///> Pointer to the node before
    struct node* prev;

    ///> Pointer to the following node
    struct node* nxt;

    ///> Void Pointer to the value
    void* value;
} Node;

/*!
 * \brief A list.
 *
 * A doubly linked list
 */
typedef struct list {
    ///> The size of the list
    int sz;

    ///> Pointer the beginning of the list
    struct node *begin;

    ///> Pointer to the beginning of the list
    struct node *end;
} List;

/*!
 *  \brief Add node to the end of the list.
 *  \param list a list.
 *  \param value a value to be stored in the node.
 *  \return nothing.
 */
void list_push_back(List*, void*);

/*!
 *  \brief Remove last node.
 *  \param list a list.
 *  \return pointer to the value of the returned
 */
void* list_pop_back(List*);

/*!
 *  \brief Create a node.
 *  \param value a void pointer to the value.
 *  \return a pointer to the node
 */
Node* node_create(void*);

/*!
 *  \brief Create a list.
 *  \return a pointer to the list created.
 */
List* list_create();

/*!
 *  \brief Destroy and free everything in the list.
 *  \param list a list.
 *  \return nothing
 */
void list_destroy(List*);