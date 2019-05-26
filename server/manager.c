#include "manager.h"

// Data
int topology_type;
int queue_id;

int up(idx) {
	switch (topology_type) {
		case TREE:
			return ft_up(idx);
		case HYPERCUBE:
			return hc_up(idx);
		case TORUS:
			return tr_up(idx);
	}
	
	return 0;
}

Msg execute_job(int idx, char *program) {
	char path[PATH_SIZE] = PATH;
	strcat(path, program);
	
	time_t start = time(NULL);
	int pid_worker = fork();
	if (pid_worker == 0) { // child of the fork
		int err = execl(path, program, (char *) 0);
	
		if (err < 0) {
			E("An error ocurred!");
			exit(1);
		}
	}

	int status;
	wait(&status);

	time_t elapsed = time(NULL) - start;

	printf("\n[%d] tempo do job: %ld\n", idx, elapsed); fflush (stdout);

	return (Msg) { 
		up(idx), 
		elapsed,
		""
	};
}

void send_msg_ft(int idx, Msg msg, int action) {
	switch (action) {
		case NEW_JOB:
			if ((2 * idx + 2) < N) {
				msg.type = 2 * idx + 1;
				msgsnd(queue_id, &msg,  sizeof(Msg), 0);
				msg.type = 2 * idx + 2;
				msgsnd(queue_id, &msg,  sizeof(Msg), 0);
			}
			break;
		case JOB_FINISHED:
			if (idx > 0) {
				msg.type = idx / 2;
				msgsnd(queue_id, &msg,  sizeof(Msg), 0);
			}
			break;
		default:
			E("Wrong message type!");
	}
}


void broadcast(int idx, Msg msg, int action) {
	switch (topology_type) {
		case TREE:
			send_msg_ft(idx, msg, action);
			break;
		case HYPERCUBE:
			//send_msg_hc(idx, msg, action);
			break;
		case TORUS:
			//send_msg_to(idx, msg, action);
			break;
		default:
			E("Wrong topology!");
			exit(1);
	}
}

void mng_on_error() {
	E("Failed to receive message");
	exit(1);
}

void mng_on_success(int idx, Msg msg) {
	broadcast(idx, msg, NEW_JOB);
	msg = execute_job(idx, msg.s);
	// broadcast(type, msg, JOB_FINISHED);
}

void receive_msg(int idx) {
	Msg msg;

	while (true) {
		int res = msgrcv(queue_id, &msg,  sizeof(Msg), idx, 0);

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
int kill_p(int * pid_array)
{
	int i;
	for(i = 0; i <= N; i++)
	{
		kill(pid_array[i],SIGTERM);
	}
}
