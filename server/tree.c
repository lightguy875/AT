#include "tree.h"

void ft_make(int *ft) {
	for (int i = 0; i < N; i++) {
		ft[i] = -1;
	}
}

int ft_up (int idx) {
  if (idx)
	  return (idx - 1) / 2;
  
  return N;
}

void ft_down(int idx, int *ans, int sz) {
	ans[0] = (2 * idx + 1 < sz) ? 2 * idx + 1 : -1;
	ans[1] = (2 * idx + 2 < sz) ? 2 * idx + 2 : -1;
}
