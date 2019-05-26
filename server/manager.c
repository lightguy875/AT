#include "manager.h"

// Data
int topology_type;
int id_queue; 

bool if_worker(int id) {
	return id == 0;
}

time_t get_time () {
	return time(NULL);
}

time_t job_time (time_t start, time_t end) {
	return end - start;
}

Msg execute_job(int idx, char *program) {
	int status, pid_worker, err = 0;
	time_t start, end;

	char path[PATH_SIZE] = PATH;
	strcat(path, program);
	
	pid_worker = fork();
	if (if_worker(pid_worker)) {
		start = get_time();
		err = execl(path, program, (char *) 0);
	}

	wait(&status);
	end = get_time();

	fflush (stdout);
	if (err < 0)
		E("An error ocurred!");

	int p_manager = ft_up(idx);
	time_t job_t = job_time(start, end);

	printf("\n[%d] tempo do job: %.5f\n", idx, (float) job_t);

	Msg msg = { p_manager, job_t };
	return msg;
}

void send_msg_ft(int idx, Msg msg, int action) {
	switch (action) {
		case NEW_JOB:
			if ((2 * idx + 2) < N) {
				msg.type = 2 * idx + 1;
				msgsnd(id_queue, &msg,  sizeof(Msg), 0);
				msg.type = 2 * idx + 2;
				msgsnd(id_queue, &msg,  sizeof(Msg), 0);
			}
			break;
		case JOB_FINISHED:
			if (idx > 0) {
				msg.type = idx / 2;
				msgsnd(id_queue, &msg,  sizeof(Msg), 0);
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
/*    case HYPERCUBE:
			send_msg_hc(idx, msg, action);
      break;
    case TORUS:
			send_msg_to(idx, msg, action);
      break; */
    default:
      E("Wrong topology!");
      exit(1);
  }
}

void receive_msg(int idx) {
	Msg msg;

	while (true) {
		int res = msgrcv(id_queue, &msg,  sizeof(Msg), idx, 0);

		if (res < 0)
			E("Failed to receive message");

		broadcast(idx, msg, NEW_JOB);
		msg = execute_job(idx, msg.s);
		// broadcast(type, msg, JOB_FINISHED);
	}
}

int get_message_queue () {
	int key = KEY;
	return msgget(key, MSG_FLAG);
}

void to_manage(int idx, int topology) {
	id_queue = get_message_queue();
	topology_type = topology;

	receive_msg(idx);
}
