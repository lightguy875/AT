#include "torus.h"

void tr_make() {
  for (int i = 0; i < M; i++) {
    for (int j = 0; j < M; j++) {
      tr[i][j] = -1;
      tr_vis[i][j] = 0;
    }
  }
}

// Torus DFS
void tr_dfs(int i, int j, int di, int dj) {
  static int nxt[] = { 1, 0, -1, 0 };
  
  if (i == di && j == dj) {
    int pid = tr[i][j];

    // do something
  }
  
  for (int i = 0; i < N; i++) {
    tr_dfs(i + nxt[i], j + nxt[(i+1)%N], di, dj);
  }
}