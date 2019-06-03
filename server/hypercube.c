#include "hypercube.h"

void hc_make(int *hc) {
	for (int i = 0; i < N; i++) {
		hc[i] = -1;
	}
}

int hc_up (int idx) {
	if (idx) {
  		return idx  - (idx&-idx);
	}

	return N;
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
void hc_down(int idx, int* ans) {
	for (int i = 0; i < 4; i++) {
		if (idx & (1 << i)) {
			break;
		} else {
			ans[i] = idx | (1 << i);
		}
	}

	for (int i = 0; i < 4; i++) {
		ans[i] = (ans[i] < N) ? ans[i] : -1;
	}
}
