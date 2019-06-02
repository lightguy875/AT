#include "hypercube.h"

void hc_make(int *hc) {
  for (int i = 0; i < N; i++) {
    hc[i] = -1;
  }
}

int hc_up (int idx) {
  return idx  - (idx&-idx);
}

void hc_down(int idx, int ans[]) {
	int n = sizeof(*ans) / sizeof(int);

	for (int i = 0; i < n; i++) {
		ans[i] = (i < 2) ? 2 * idx + i + 1 : -1;
	}
}