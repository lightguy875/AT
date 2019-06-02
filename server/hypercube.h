#pragma once

#include "util.h"

/*!
 *  \brief Initializa the structure
 *  \param hc an array to store the structure.
 */
void hc_make(int *hc);

/*!
 *  \brief Get the index of the father of the node at the given index.
 *  \param idx the index.
 *  \return The index of the father
 */
int hc_up (int idx);

/*!
 *  \brief Get the index of the sons of the node at the given index.
 *  \param idx the index.
 *  \param ans the output, it should be at least 2, the rest will be filled with -1.
 */
void hc_down(int idx, int ans[]);
