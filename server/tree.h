#pragma once

#include "util.h"
/**
 * @brief Create a new tree structure
 * 
 * @param ft an array to start the structure 
 */

void ft_make(int *ft);
/**
 * @brief Access up values from fat tree
 * 
 * @param idx index
 * @return int 
 */

int ft_up (int idx);
/**
 * @brief Get the index of the sons of the node at the given index.
 * 
 * @param idx  the index.
 * @param ans the output, it should be at least 2, the rest will be filled with -1.
 */
void ft_down(int idx, int *ans);
