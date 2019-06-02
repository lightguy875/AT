#include "torus.h"

void tr_make(int *tr) {
	for (int i = 0; i < N; i++) {
		tr[i] = -1;
	}
}

int tr_up (int idx) {
	int i = idx % M;
	int j = idx / M;

	if (i) {
		return idx - M;
	}

	return idx - 1;
}

void tr_down(int idx, int ans[]) {
	int n = sizeof(*ans) / sizeof(int);

	for (int i = 0; i < n; i++) {
		ans[i] = (i < 2) ? 2 * idx + i + 1 : -1;
	}
}