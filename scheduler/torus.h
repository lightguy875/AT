#pragma once

#include "util.h"

static int tr[M][M];

static bool tr_vis[M][M];

// Torus Make
void tr_make();

// Torus DFS
void tr_dfs(int i, int j, int di, int dj);