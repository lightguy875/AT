#include "torus.h"

void tr_make(int *tr) {
	for (int i = 0; i < N; i++) {
		tr[i] = -1;
	}
}

/* Structure: root node 1
12 13 14 15  
8  9  10 11
4  5  6  7
0  1  2  3   // First line
*/
int tr_up (int idx) {
	if (idx > 1) { // if isn't the root node
		if(idx >= M * 3) {// if top line, send to first line
			return (idx + M) % N; 
		} else if (idx % M == 3) { // if last right column, send to first column
			return (idx - M + 1); 
		} else if (idx % M == 0) { // if first column, send to the below node
			return idx - M;
		} else { // if first line or mid, go to the previous left node
			return idx - 1;
		}
	} else { // if root node, send to scheduler
		return N;
	}
}

void tr_down(int idx, int ans[]) {
	int n = sizeof(*ans) / sizeof(int);

	for (int i = 0; i < n; i++) {
		if (i) {
			ans[i] = -1;
		} else {
			if (idx < M - 1) { // first send to node 2, 3 and 4
				ans[0] = idx + 1;
			}

			if (idx < N - M) { // nodes 1, 2, 3 and 4 sendo to above nodes
				ans[0] = idx + M;
			}
		}
	}
}