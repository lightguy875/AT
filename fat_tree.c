#define N 16
#define T int // to make clear that we are dealing with PID of a process

T fat_tree[N];

void dfs (int idx, T* fat_tree) {
  if (idx >= N) {
    return;
  }
  
  int nxt = 2 * idx;
  int connection = idx < 3 ? 2 : 1;
  
  for (int i = 1; i <= connection; i++) {
    dfs(nxt+1, fat_tree);
    dfs(nxt+2, fat_tree);
  }
}
