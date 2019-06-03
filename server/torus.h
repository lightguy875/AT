#pragma once
/**
 * @file torus.h
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

/**
 * @brief Initializa the structure
 * 
 * @param tr an array to store the structure.
 */
void tr_make(int *tr);
/**
 * @brief Get the index of the father of the node at the given index.
 * 
 * @param idx the index.
 * @return int  The index of the father
 */
int tr_up(int idx);
/**
 * @brief Get the index of the sons of the node at the given index.
 * 
 * @param idx the index.
 * @param ans the output, it should be at least 2, the rest will be filled with -1.
 */
void tr_down(int idx, int ans[]);
