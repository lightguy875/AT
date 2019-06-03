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

List* jobs;

void shutdown() {
	Node *node = jobs->begin;

	while (node != NULL) {
		Job *job = (Job*)node->value;

		if(job->done) {
			printf("\n Processo já executado \n");
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
 * @param idx 
 * @param program 
 * @return Msg 
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
	printf("Process %d: job done in %d sec.\n", idx, (int) elapsed);

	return (Msg) { 
		0, 
		elapsed,
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
		// printf("idx: %d - arr: %d\n", idx, arr[i]);
		if (arr[i] != -1) {
			printf("%d:%ld\n", idx + 1, msg.type);
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
void msn_start(int idx) {
	queue_id = queue_retrieve(KEY);

	while (true) {
		Msg msg;

		int res = msgrcv(queue_id, &msg, sizeof(Msg) - sizeof(long), idx, 0);

		if (res < 0) {
			E("Failed to receive message. A process was killed...");
			break;
		} else {
			// TODO: identify what is this
			//	if (strcmp(msg.s, "finished")) { // if ins't a new message
			if (strstr (msg.s, "finished") == NULL) {
				mng_broadcast_down(idx, msg);
				msg = mng_execute(idx, msg.s);
			}
			
			char buffer[33];
			
			sprintf(buffer, " -> %d", idx);
			strcat(msg.s, buffer);

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
		if (!fork()) {
			msn_start(i);
		}
	}
}

/**
 * @brief Mark a job as done given an id
 * 
 * @param id The id of the job
 */
void sch_mark_job_done (int id) {
  Node *curr = jobs->begin;

  while (curr != NULL) {
		Job *aux = (Job*) curr->value;

	  if (aux != NULL && aux->id == id) {
		  aux->done = true;
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

		if (job == NULL || job->seconds > aux->seconds) {
			job = aux;
		}

		curr = curr->nxt;
	}

	return (job != NULL && job->seconds <= time(NULL)) ? job : NULL;
}

/**
 * @brief Send message to node 0 to start execution
 * 
 * @param job The job to be executed
 */
void sch_execute (Job* job) {
	Msg msg;

	msg.t = 0;
	msg.type = 1;
	strcpy(msg.s, job->filename);
	
	msgsnd(queue_id, &msg, sizeof(Msg) - sizeof(long), 0);
	// TODO: wait for message sent by node 0
}

/**
 * @brief Try to treat error that might be caused by alarm
 */
void sch_msg_error() {
	if (errno == EINTR && alarm(0) == 0) { // Checked if the interruption error was caused by an alarm (deactivates the alarm in the process)
		Job *nxt_job = sch_get_next_job();
		sch_execute(nxt_job);
	} else { // The interruption error was not caused by the alarm or failed to receive (no interruption error)
		E("Failed to receive message");
		exit(1);
	}
}

/**
 * @brief Check if should execute any program and if necessary, it will execute.
 */
void sch_check_and_run () {
	Job *nxt_job = sch_get_next_job();
	time_t now = time(NULL);

	if (nxt_job->seconds <= now) { // Deactivate the alarm and execute if it was to execute now the file
		// TODO: check what happens if we sent a program to execute during another execution
		alarm(0);
		sch_mark_job_done(nxt_job->id);
		sch_execute(nxt_job);
		// check_run(queue_id);
	} else { // Modify the alarm for the new job, since it is closer to execute
		alarm(nxt_job->seconds - now);
	}
}

/**
 * @brief Treat a successfull message coming
 * 
 * @param msg The message received
 */
void sch_msg_success(Msg msg) {
	Job *job = job_create(msg.t, msg.s);

	list_push_back(jobs, job);

	sch_check_and_run();
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

		int res = msgrcv(queue_id, &msg, sizeof(Msg) - sizeof(long), virtual_id, 0);

		if (strstr(msg.s, "finished") != NULL) {
			strcat(traces, msg.s);
			strcat(traces, " -> SCHEDULER\n");
			strcpy(msg.s, traces);

			if (cont == N) {
				S("...Job finished... ");
				printf("\n=> Traces\n%s\n", msg.s);
			} 

			cont = (cont + 1) % N;
		} else {
			if (res < 0) {
				sch_msg_error();
			} else {
				sch_msg_success(msg);
			}
		}
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
	signal(SIGALRM, *dummy);
	S("Alarm signal set");

	signal(SIGUSR1,*shutdown);
	S("Shutdown signal set");

	queue_id = queue_create(KEY);
	S("Queue set");

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

	mng_create(N);
	S("Create Managers");

	sch_start();

	return 0;
}