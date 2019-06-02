#include "hypercube.h"

void hc_make(int *hc) {
	for (int i = 0; i < N; i++) {
		hc[i] = -1;
	}
}

int hc_up (int idx) {
	if (idx < 1) {
		return N+1;
	}

  	return idx  - (idx&-idx);
}

/*
0000 -> 0001
        0010 -> 0011
        0100 -> 0101
                0110
        1000 -> 1001
                1010 -> 1011
                1100 -> 1101
                        1110 -> 1111
*/
void hc_down(int idx, int ans[]) {
	int n = sizeof(*ans) / sizeof(int);

	for (int i = 0; i < n; i++) {
		ans[i] = -1;
	}

	for (int i = 0; i < 4; i++) {
		if (idx & (1 << i)) {
			ans[i] = idx | (1 << i);
		} else {
			break;
		}
	}
}
