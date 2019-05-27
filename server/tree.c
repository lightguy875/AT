#include "tree.h"
#include "manager.h"

// Tree Make
void ft_make(int *ft, int *vis) {
  for (int i = 0; i < N; i++) {
    ft[i] = -1;
    vis[i] = 0;
  }
}

// Tree DFS
void ft_dfs(int *ft, int idx, int destiny) {
  if (idx >= N) {
    return;
  }

  int pid;
  if (idx == destiny){
    // got on destiny
  }
  
  int nxt = 2 * idx;
	
  ft_dfs(ft, nxt+1, destiny);
  ft_dfs(ft, nxt+2, destiny);
}

// Tree Up
// int ft_up (int idx) {
//   return idx / 2;
// }