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

List* jobs;

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
void msn_start(int idx) {
	queue_id = queue_retrieve(KEY);

	while (true) {
		Msg msg;

		int res = msgrcv(queue_id, &msg, sizeof(Msg) - sizeof(long), idx + 1, 0);
		
		if (res < 0) {
			E("Failed to receive message. A process was killed...");
			break;
		} else {
			if (strstr (msg.s, "finished") == NULL) {
				mng_broadcast_down(idx, msg);
				msg = mng_execute(idx, msg.s);
			}
			
			char buffer[33];
			
			sprintf(buffer, " -> %d", idx + 1);
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
 * @brief Generates a report about the job executed
 * 
 * @param job The job executed
 */
char * sch_create_report(Job * job) {
	char * report = (char *) malloc(100);

	sprintf(report, "job=%d, arquivo=%s, delay=%d segundo(s), makespan: %d segundo(s)", job->id, job->filename, job->delay, job->makespan);
	return report;
}

/**
 * @brief Mark a job as done given an id
 * 
 * @param id The id of the job
 */
void sch_mark_job_done (int id, int makespan) {
	Node *curr = jobs->begin;

  	while (curr != NULL) {
		Job *aux = (Job*) curr->value;

		if (aux != NULL && aux->id == id) {
			aux->done = true;
			aux->makespan = makespan;
			 
			strcpy(aux->report, sch_create_report(aux));
			printf("\n*** Report ***\n%s\n\n", aux->report);
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
	Msg msg;

	msg.t = 0;
	msg.type = 1;
	strcpy(msg.s, job->filename);

	job_executed = job->id;
	t_init = time(NULL);

	msgsnd(queue_id, &msg, sizeof(Msg) - sizeof(long), 0);
	topology_free = false;
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
	if (nxt_job && topology_free) {
		time_t now = time(NULL);


		if (nxt_job->seconds <= now) { // Deactivate the alarm and execute if it was to execute now the file
			printf("[SCHEDULER] Executing the Job %d...\n\n", nxt_job->id);
			alarm(0);
			sch_execute(nxt_job);
		} else { // Modify the alarm for the new job, since it is closer to execute
			printf("[SCHEDULER] %ld seconds to execute the Job %d\n\n", nxt_job->seconds - now, nxt_job->id);
			printf("ANTES...\n");
			printf("ALARME: %d\n", alarm(nxt_job->seconds - now));
			printf("DEPOIS...\n");
		}
	}
}

/**
 * @brief Treat a successfull message coming
 * 
 * @param msg The message received
 */
void sch_msg_success(Msg msg) {
	Job *job = job_create(msg.t, msg.s, msg.delay);

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
		printf("claudio espera\n");
		int res = msgrcv(queue_id, &msg, sizeof(Msg) - sizeof(long), virtual_id, 0);
		printf("claudio recebe\n");
		if (strstr(msg.s, "finished") != NULL) {

			strcat(traces, msg.s);
			strcat(traces, " -> SCHEDULER\n");
			strcpy(msg.s, traces);

			if (cont == N - 1) {
				S("\n...Job finished... ");
				strcpy(traces, "");
				// printf("\n=> Traces\n%s\n", msg.s);
				int makespan = (int) time(NULL) - t_init;
				sch_mark_job_done(job_executed, makespan);
				topology_free = true;
				sch_check_and_run();
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

	topology_free = true;
	
	mng_create(N);
	S("Create Managers");

	sch_start();

	return 0;
}