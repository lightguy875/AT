#include "fat_tree.h"
#include "manager.h"

// Fat Tree Make
void ft_make(int *ft, int *vis) {
  for (int i = 0; i < N; i++) {
    ft[i] = -1;
    vis[i] = 0;
  }
}

// Fat Tree DFS
void ft_dfs(int *ft, int idx, int destiny) {
  if (idx >= N) {
    return;
  }

	int pid;
  if (idx == destiny){
  //
  }
  
  int nxt = 2 * idx;
  // int connection = idx < 3 ? 2 : 1;
  
  //for (int i = 1; i <= connection; i++) {
  ft_dfs(ft, nxt+1, destiny);
  ft_dfs(ft, nxt+2, destiny);
  //}
}

// Fat Tree Up
int ft_up (int idx) {
  return idx / 2;
}