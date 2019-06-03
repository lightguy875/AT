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
	int n = sizeof(ans) / sizeof(ans[0]);

	for (int i = 0; i < n; i++) {
		ans[i] = (i < 2) ? 2 * idx + i + 1 : -1;
		ans[i] = ans[i] > N ? -1 : ans[i];
		printf("ansi: %d - i: %d - n: %ld\n", ans[i], i, n);
	}
}
