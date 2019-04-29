#define T int

T* hypercube[N]; // store the pids

// do not use, it will never finish its search
void dfs (int idx, T* hypercube) {
  static int nxt[] = { 0x0001, 0x0002, 0x0004, 0x0010 };
  
  int pid = hypercube[idx];
  
  for (int i = 0; i < 4; i++) {
    dfs(idx ^ nxt[i]);
  }
}
