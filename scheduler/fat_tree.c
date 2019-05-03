#include "fat_tree.h"

// Fat Tree Make
void ft_make() {
  for (int i = 0; i < N; i++) {
    ft[i] = -1;
    ft_vis[i] = 0;
  }
}

// Fat Tree DFS
void ft_dfs(int idx, int destiny) {
  if (idx >= N) {
    return;
  }

  if (idx == destiny) {
    // do something
  }
  
  int nxt = 2 * idx;
  int connection = idx < 3 ? 2 : 1;
  
  for (int i = 1; i <= connection; i++) {
    ft_dfs(nxt+1, destiny);
    ft_dfs(nxt+2, destiny);
  }
}