#include "torus.h"

void tr_make(int *tr, int *vis) {
  for (int i = 0; i < N; i++) {
    tr[i] = -1;
    vis[i] = 0;
  }
}

// Torus DFS
void tr_dfs(int *tr, int idx, int destiny) {
  static int nxt[] = { 1, 0, -1, 0 };
  
  if (idx == destiny) {
    int pid = tr[idx];

    // do something
  }
  
  for (int i = 0; i < M; i++) {
    int i = ((idx % M) + nxt[i] + N) % N;
    int j = ((idx / M) + nxt[(i+1)%N] + N) % N;

    tr_dfs(tr, (j * M) + i, destiny);
  }
}

// Torus Up
int tr_up (int idx) {
  int i = idx % M;
  int j = idx / M;

  if (i) {
    return idx - M;
  }

  return idx - 1;
}