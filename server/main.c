//	TODO: shutdown, estatística, verificar pedido de múltiplos jobs


#include "util.h"

#include "job.h"
#include "list.h"
#include "message.h"

#include "tree.h"
#include "hypercube.h"
#include "torus.h"

int topology_type;

int structure[N];

int queue_id;

int job_executed;

int t_init;

bool topology_free;

int pids[N];

List* jobs;

char buffer[33];

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

	return (Msg) { 
		0,
		elapsed,
		0,
		idx+1,
		"finished"
	};
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

		if (strstr (msg.s, "finished") == NULL) {
			mng_broadcast_down(idx, msg);
			msg = mng_execute(idx, msg.s);
		}
		
		sprintf(buffer, " -> %d", idx + 1);
		strcat(msg.s, buffer);

		mng_broadcast_up(idx, msg);
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
 * @brief Mark a job as done given an id and generate a report about it
 * 
 * @param id The id of the job
 */
void sch_mark_job_done (int id) {
	topology_free = true;

	Node *curr = jobs->begin;

  	while (curr != NULL) {
		Job *aux = (Job*) curr->value;

		if (aux != NULL && aux->id == id) {
			aux->done = true;
			aux->makespan = (int) time(NULL) - t_init;

			printf("\n> Job Report\n");
			printf("Job=%d, Arquivo=%s, Delay=%ds, Makespan: %ds\n", aux->id, aux->filename, aux->delay, aux->makespan);
		  	break;
	  	}

	  	curr = curr->nxt;
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
 * 
 * @param job The job to be executed
 */
void sch_execute (Job* job) {
	printf("[SCHEDULER] Executing the Job %d...\n\n", job->id);

	Msg msg;

	msg.t = 0;
	msg.type = 1;
	strcpy(msg.s, job->filename);

	job_executed = job->id;
	t_init = time(NULL);

	msgsnd(queue_id, &msg, sizeof(Msg) - sizeof(long), IPC_NOWAIT);
	topology_free = false;
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
	static int count = 0;

	if (msg.origin < N) { // message from sons
		if (count == N - 1) {
			sch_mark_job_done(job_executed);
		} 

		count = (count + 1) % N;
	} else { // message from clients
		Job *job = job_create(msg.t, msg.s, msg.delay);

		list_push_back(jobs, job);
	}
}

void sch_try_execute() {
	if (topology_free) {
		Job *nxt_job = sch_get_next_job();
		time_t now = time(NULL);

		if (nxt_job && nxt_job->seconds <= now) {
			sch_execute(nxt_job);
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