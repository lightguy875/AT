#pragma once

#include "util.h"

static int hc[N];

static bool hc_vis[N];

// Hypercube Make
void hc_make();

// Hypercube DFS
void hc_dfs(int idx, int destiny);