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

void ft_down(int idx, int *ans) {
	
	for (int i = 0; i < 4; i++) {
		ans[i] = (i < 2) ? 2 * idx + i + 1 : -1;
		ans[i] = ans[i] > N - 1 ? -1 : ans[i];
	}
}
