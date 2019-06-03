#pragma once
/**
 * @file list.h
 * @author Luís Eduardo Luz Sila - 15/0137885
 * @author Claudio Segala Rodrigues Filho 15/0032552
 * @author Yan Victor dos Santos 14/0033599
 * @brief 
 * @version 0.1
 * @date 2019-06-03
 * 
 * @copyright Copyright (c) 2019
 * 
 */
#include "util.h"
#include "job.h"
/**
 * @brief A node.
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
/**
 * @brief A list.
 *  A doubly linked list
 */

typedef struct list {
    ///> The size of the list
    int sz;

    ///> Pointer the beginning of the list
    struct node *begin;

    ///> Pointer to the beginning of the list
    struct node *end;
} List;
/**
 * @brief Create a node.
 * 
 * @param value a void pointer to the value.
 * @return Node* a pointer to the node
 */
Node* node_create(void* value);
/**
 * @brief Create a list.
 * 
 * @return List* a pointer to the list created.
 */
List* list_create();
/**
 * @brief  Add node to the end of the list.
 * 
 * @param list a list.
 * @param value a value to be stored in the node.
 */
void list_push_back(List* list, void* value);
/**
 * @brief Remove last node.
 * 
 * @param list a list.
 * @return void*  pointer to the value of the returned
 */
void* list_pop_back(List* list);
/**
 * @brief Destroy and free everything in the list.
 * 
 * @param list 
 */

void list_destroy(List* list);