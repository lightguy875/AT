#include "torus.h"

void tr_make(int *tr) {
	for (int i = 0; i < N; i++) {
		tr[i] = -1;
	}
}
/**
 * @brief Estrutura do no
 * 
 * Structure: root node 1
	12 13 14 15  
	8  9  10 11
	4  5  6  7
	0  1  2  3   // First line
 * 
 */
int tr_up (int idx) {
	if (idx) { // if isn't the root node
		if(idx > M) {// if top line, send to first line
			return idx - M; 
		} else {
			return idx - 1;
		}
	} else { // if root node, send to scheduler
		return N;
	}
}
/**
 * @brief Enviar para os nós embaixo da árvore
 * 
 * @param idx 
 * @param ans 
 */
void tr_down(int idx, int* ans) {
	ans[0] = idx + M;

	if (idx < M - 1) {
		ans[1] = idx + 1;
	}

	ans[0] = ans[0] < N ? ans[0] : -1;
	ans[1] = ans[1] < N ? ans[1] : -1;
}