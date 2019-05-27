#include "manager.h"
int cont = 0;
// Data
int topology_type;
int queue_id;

Msg execute_job(int idx, char *program) {

	time_t start = time(NULL);
	int pid_worker = fork();
	if (pid_worker == 0) { // child of the fork
		int err = execl(PATH, program, (char *) 0);
	
		if (err < 0) {
			E("An error ocurred!");
			exit(1);
		}
	}

	int status;
	wait(&status);

	time_t elapsed = time(NULL) - start;
	printf("[%d]Job executado: %ld\n", idx, elapsed);
	return (Msg) { 
		0, 
		elapsed,
		"finished"
	};
}

int ft_down (int idx, Msg msg) {
	if ((2 * idx + 1) < 4) {
		msg.type = 2 * idx;
		msgsnd(queue_id, &msg,  sizeof(Msg), 0);
		msg.type = 2 * idx + 1;
		msgsnd(queue_id, &msg,  sizeof(Msg), 0);
	}
}

int tr_down (int idx, Msg msg) {
  // static int nxt[] = { 1, 0, -1, 0 };
    
  // for (int i = 0; i < M; i++) {
  //   int i = ((idx % M) + nxt[i] + N) % N;
  //   int j = ((idx / M) + nxt[(i+1)%N] + N) % N;

  //   tr_dfs(tr, (j * M) + i, destiny);
  // }
}

int ft_up (int idx, Msg msg) {

	if (idx > 1) {
		msg.type = idx / 2;
		msgsnd(queue_id, &msg,  sizeof(Msg), 0);
	} else {
		msg.type = N + 1;
		msgsnd(queue_id, &msg, sizeof(Msg), 0);
	}
}

int tr_up (int idx, Msg msg) {
	int i = idx % M;
  int j = idx / M;

  if (i) {
    msg.type = idx - M;
  } else {
		msg.type = idx - 1;
	}

	msgsnd(queue_id, &msg,  sizeof(Msg), 0);
}

int br_down(int idx, Msg msg) {
	switch (topology_type) {
		case TREE:
			return ft_down(idx, msg);
		// case HYPERCUBE:
		// 	return hc_down(idx);
		// case TORUS:
		// 	return tr_down(idx);
	}

	return 0;
}

int br_up(int idx, Msg msg) {
	switch (topology_type) {
		case TREE:
			return ft_up(idx, msg);
		// case HYPERCUBE:
		// 	return hc_up(idx, msg);
		// case TORUS:
		// 	return tr_up(idx, msg);
	}

	return 0;
}

void mng_on_success(int idx, Msg msg) {

	if (strcmp(msg.s, "finished")) {
		br_down(idx, msg);
		msg = execute_job(idx, msg.s);
		printf("[%d] %s.\n", idx, msg.s);
	}

	br_up(idx, msg);
}

void mng_on_error() {
	E("Failed to receive message");
	printf("morrendo\n");
	fflush(stdout);
	exit(1);
}

void receive_msg(int idx) {
	Msg msg;

	while (true) {

		int res = msgrcv(queue_id, &msg, sizeof(Msg), idx, 0);

		if (res < 0) {
			mng_on_error();
		} else {
			mng_on_success(idx, msg);
		}
	}
}

void to_manage(int idx, int topology) {
	queue_id = queue_retrieve(KEY);

	topology_type = topology;
	receive_msg(idx);
}

// int kill_p(int * pid_array)
// {
// 	int i;
// 	for(i = 0; i <= N; i++)
// 	{
// 		kill(pid_array[i],SIGTERM);
// 	}
// }

