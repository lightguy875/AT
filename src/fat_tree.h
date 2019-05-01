#pragma once

#include "util.h"

static int ft[N];

static bool ft_vis[N];

// Fat Tree Make
void ft_make();

// Fat Tree DFS
void ft_dfs(int idx, int destiny);