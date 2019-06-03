//	TODO: shutdown, estatística, verificar pedido de múltiplos jobs

#include "util.h"

#include "job.h"
#include "list.h"
#include "message.h"

#include "tree.h"
#include "hypercube.h"
#include "torus.h"

#define SCHEDULER 666

int topology_type;

int structure[N];

int queue_id;

bool topology_free;

int pids[N];

List* jobs;

Job* curr_job;

void shutdown() {
	Node *node = jobs->begin;

	while (node != NULL) {
		Job *job = (Job*)node->value;

		if(job->done) {
			printf("ID: %d Arquivo : %s Tempo de execução %d", job->id, job->filename, job->seconds);	 
		} else{
			printf(" \n O processo: %d não será executado \n", job->id);
		}

		node = node->nxt;
	}
	
	kill(0,SIGTERM);
}

/**
 * @brief Destroy the list of jobs and the queue
 */
void destroy() {
	list_destroy(jobs);

	struct msqid_ds msg;
	msgctl(queue_id, IPC_RMID, &msg);
}

/**
 * @brief Execute a program
 * 
 * @param idx The current index
 * @param program The name of the program
 * @return Msg Message with the elapsed time
 */
Msg mng_execute(int idx, char *program) {
	time_t start = time(NULL);

	int pid_worker = fork();
	if (pid_worker == 0) { // child of the fork
		char path[100] = PATH;
		strcat(path, program);		
		int err = execl(path, program, (char *) 0);
		
		if (err < 0) {
			E("An error ocurred!");
			exit(1);
		}
	}

	int status;
	wait(&status);
	time_t elapsed = time(NULL) - start;

	Msg msg;

	msg.type = 0;
	msg.id = getpid();
	msg.t = elapsed;
	msg.delay = 0;
	msg.origin = idx;
	strcpy(msg.s, "finished");

	return msg;
}

/**
 * @brief Send message down the structure
 * 
 * @param idx The current index
 * @param msg The message to be sent
 */
void mng_broadcast_down(int idx, Msg msg) {
	int arr[4] = { -1, -1, -1, -1 };

	switch (topology_type) {
		case TREE:
			ft_down(idx, arr);
			break;
		case HYPERCUBE:
			hc_down(idx, arr);
			break;
		case TORUS:
			tr_down(idx, arr);
			break;
	}

	for (int i = 0; i < 4; i++) {
		if (arr[i] != -1) {
			msg.type = arr[i] + 1;
			msgsnd(queue_id, &msg,  sizeof(Msg) - sizeof(long), IPC_NOWAIT);
		}
	}
}

/**
 * @brief Send message up the structure
 * 
 * @param idx The current index
 * @param msg The message to be sent
 */
void mng_broadcast_up(int idx, Msg msg) {
	switch (topology_type) {
		case TREE:
			msg.type = ft_up(idx);
			break;
		case HYPERCUBE:
			msg.type = hc_up(idx);
			break;
		case TORUS:
			msg.type = tr_up(idx);
			break;
	}

	msg.type++;
	msgsnd(queue_id, &msg, sizeof(Msg) - sizeof(long), IPC_NOWAIT);
}

/**
 * @brief Start a manager
 */
void mng_start(int idx) {
	while (true) {
		Msg msg;

		int virtual_queue = idx+1;
		int res = msgrcv(queue_id, &msg, sizeof(Msg) - sizeof(long), virtual_queue, 0);
		
		if (res < 0) {
			E("Failed to receive message. Maybe the queue died?");
			break;
		}

		if (msg.origin == SCHEDULER) {
			mng_broadcast_down(idx, msg);
			msg = mng_execute(idx, msg.s);
			mng_broadcast_up(idx, msg);
		} else {
			mng_broadcast_up(idx, msg);
		}
	}

	exit(1);
}

/**
 * @brief Creation of all management processes.
 */
void mng_create (int n) {
	for (int i = 0; i < n; i++) {
		pid_t pid = fork();

		if (!pid) {
			mng_start(i);
		} else {
			pids[i] = pid;
		}
	}
}

/**
 * @brief Retrieves the oldest job that wasn't executed
 * 
 * @return Job* a pointer to the next job
 */
Job* sch_get_next_job () {
	Job *job = NULL;
	Node *curr = jobs->begin;

	while (curr != NULL) {
		Job *aux = (Job*) curr->value;

		if ((job == NULL || job->seconds > aux->seconds) && !aux->done) {
			job = aux;
		}

		curr = curr->nxt;
	}

	return job;
}

/**
 * @brief Send message to node 0 to start execution
 */
void sch_execute () {
	printf("[SCHEDULER] Executing the Job %d\n\n", curr_job->id);
	
	topology_free = false;
	curr_job->start = time(NULL);

	Msg msg;

	msg.type = 1;
	msg.id = 0;
	msg.t = 0;
	msg.delay = 0;
	msg.origin = SCHEDULER;
	strcpy(msg.s, curr_job->filename);

	msgsnd(queue_id, &msg, sizeof(Msg) - sizeof(long), 0);
}

/**
 * @brief Try to treat error that might be caused by alarm
 */
void sch_msg_error() {
	if (errno != ENOMSG) {
		E("Failed to receive message");
		exit(1);
	}
}

/**
 * @brief Treat a successfull message coming
 * 
 * @param msg The message received
 */
void sch_msg_success(Msg msg) {
	printf("\n\n> type: %ld, id: %ld, seconds: %d(Since 70's), delay: %ds, message: %s, origin: %d\n\n", msg.type, msg.id, msg.t, msg.delay, msg.s, msg.origin);

	if (msg.origin < N) { // message from sons
		curr_job->node_pid[msg.origin] = msg.id;
		curr_job->node_time[msg.origin] = msg.t;
		curr_job->completed++;

		if (curr_job->completed == N) { // finished the job
			topology_free = true;
			curr_job->done = true;
			curr_job->finish = time(NULL);
			
			printf("\n> job=%d, arquivo=%s, delay=%ds, makespan: %ds\n", curr_job->id, curr_job->filename, curr_job->delay, curr_job->finish - curr_job->start);
		}
	} else { // message from clients
		Job *job = job_create(msg.id, msg.t, msg.s, msg.delay);

		list_push_back(jobs, job);
	}
}

void sch_try_execute() {
	if (topology_free) {
		Job *nxt_job = sch_get_next_job();
		time_t now = time(NULL);

		if (nxt_job && nxt_job->seconds <= now) {
			curr_job = nxt_job;
			sch_execute();
		}
	}
}

/**
 * @brief Start a scheduler
 */
void sch_start () {
	int cont = 0;
	int virtual_id = N+1; //> the virtual queue id
	char traces[1000];

	while (true) {
		Msg msg;
		int res = msgrcv(queue_id, &msg, sizeof(Msg) - sizeof(long), virtual_id, IPC_NOWAIT);

		if (res < 0) {
			sch_msg_error();
		} else {
			sch_msg_success(msg);
		}

		sch_try_execute();
	}

	destroy();
}

/**
 * @brief Setup everything, create the managers and start the scheduler
 * 
 * @param argc The number of arguments
 * @param argv The array of arguments
 * @return int If there was an error
 */
int main (int argc, char *argv[]) {
	signal(SIGUSR1, shutdown);
	S("Shutdown signal set");

	queue_id = msgget(KEY, IPC_CREAT | 0777);

    if (queue_id < 0) {
        E("Failed to get queue");
    } else {
		S("Queue set");
	}

	jobs = list_create();
	S("List of jobs set");

	if (argc != 2 || !try_cast_int(argv[1], &topology_type)) {
		E("Not a valid topology");
		exit(1);
	}

	switch (topology_type) {
		case TREE:
			ft_make(structure);
			break;
		case HYPERCUBE:
			hc_make(structure);
			break;
		case TORUS:
			tr_make(structure);
			break;
		default:
			E("Wrong topology!");
			exit(1);
	}

	S("Topology set");

	topology_free = true;
	
	mng_create(N);
	S("Create Managers");
	sch_start();

	return 0;
}