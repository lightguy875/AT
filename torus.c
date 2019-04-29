#define N 4
#define T int // to make clear that we will be working with PID

T torus[N][N];

void dfs (int i, int j, T** torus) {
  int nxt[] = { 1, 0, -1, 0 };
  
  int pid = torus[i][j];
  
  for (int i = 0; i < N; i++) {
    dfs(i + nxt[i], j + nxt[(i+1)%N], torus);
  }
}
