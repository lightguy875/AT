#include "tree.h"

void ft_make(int *ft) {
	for (int i = 0; i < N; i++) {
		ft[i] = -1;
	}
}

int ft_up (int idx) {
	return (idx - 1) / 2;
}

void ft_down(int idx, int ans[]) {
	int n = sizeof(*ans) / sizeof(int);

	for (int i = 0; i < n; i++) {
		ans[i] = (i < 2) ? 2 * idx + i + 1 : -1;
	}
}
