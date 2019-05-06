#include "hypercube.h"

void hc_make(int *hc, int *vis) {
  for (int i = 0; i < N; i++) {
    hc[i] = -1;
    vis[i] = 0;
  }
}

// Hypercube DFS
void hc_dfs(int *hc, int idx, int destiny) {
  static int nxt[] = { 0x0001, 0x0002, 0x0004, 0x0010 };
  
  if (idx == destiny) {
    int pid = hc[idx];

    // do something
  }
  
  for (int i = 0; i < 4; i++) {
    hc_dfs(hc, idx ^ nxt[i], destiny);
  }
}

int hc_up (int idx) {
  return idx  - (idx&-idx);
}