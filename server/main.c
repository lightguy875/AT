#include "util.h"

#include "job.h"
#include "list.h"
#include "message.h"

#include "tree.h"
#include "hypercube.h"
#include "torus.h"

// Data
int pid;
int topology_type;

int pids[N];
int structure[N];

int cont = 0;
int queue_id;

char traces[1000];

/**
 * @brief 
 * 
 * @param idx 
 * @param program 
 * @return Msg 
 */
Msg execute_job(int idx, char *program) {
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

void shutdown(List* jobs)
{
	Job* Processo;
	Node* job_list = jobs->begin;
	for(job_list = jobs->begin,Processo = (Job * ) job_list->value; job_list->nxt == NULL; job_list = job_list->nxt,Processo = (Job * ) job_list->value)
	{
	 if(Processo->done)
	 {
		 printf("\n Processo já executado \n");
		 printf("ID: %d Arquivo : %s Tempo de execução %d" ,Processo->id,Processo->filename,Processo->seconds);	 
	 }
	 else
	 {
		 printf(" \n O processo: %d não será executado \n",Processo->id);
	 }
	   
	}
	kill(0,SIGTERM);
}
void shutdown_setup()
{
	signal(SIGUSR1,*shutdown);
}
void broadcast_down(int idx, Msg msg) {
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

void broadcast_up(int idx, Msg msg) {
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

void to_manage(int idx) {
	Msg msg;

	queue_id = queue_retrieve(KEY);

	while (true) {
		int res = msgrcv(queue_id, &msg, sizeof(Msg) - sizeof(long), idx, 0);

		if (res < 0) {
			E("Failed to receive message. A process was killed...");
			break;
		} else {
			// TODO: identify what is this
			//	if (strcmp(msg.s, "finished")) { // if ins't a new message
			if (strstr (msg.s, "finished") == NULL) {
				broadcast_down(idx, msg);
				msg = execute_job(idx, msg.s);
			}
			
			char buffer[33];
			
			sprintf(buffer, " -> %d", idx);
			strcat(msg.s, buffer);

			broadcast_up(idx, msg);
		}
	}

	exit(1);
}

// -------------- End of Managers

void destroy(List *jobs, int queue_id) {
  list_destroy(jobs);

	struct msqid_ds msg;
  msgctl(queue_id, IPC_RMID, &msg);
}

void remove_next_job (List* jobs, int id) {
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

Job* next_job (List* jobs) {
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

void run (Job* job, int queue_id) {
	// Send message to node 0 to start execution
	Msg msg;

	msg.t = 0;
	msg.type = 1; // To send message to node 0
	strcpy(msg.s, job->filename);
	
	fflush(stdout);
	msgsnd(queue_id, &msg, sizeof(Msg) - sizeof(long), 0);
	// TODO: wait for message sent by node 0
}

void onError(List* jobs, int queue_id) {
	if (errno == EINTR && alarm(0) == 0) {  // https://stackoverflow.com/questions/41474299/checking-if-errno-eintr-what-does-it-mean
		// Checked if the interruption error was caused 
		// by an alarm (deactivates the alarm in the process)
		
		Job *nxt_job = next_job(jobs);
		run(nxt_job, queue_id);
	} else {
		// The interruption error was not caused by the alarm or 
		// failed to receive (no interruption error)

		E("Failed to receive message");
		exit(1);
	}
}

void check_run (List* jobs, int queue_id) {
	Job *nxt_job = next_job(jobs);
	time_t now = time(NULL);

	if (nxt_job->seconds <= now) {
		// Deactivate the alarm and execute if it was
		// to execute now the file

		alarm(0);
		remove_next_job(jobs, nxt_job->id);
		run(nxt_job, queue_id);
		// check_run(jobs, queue_id);
	} else {
		// Modify the alarm for the new job, since it is 
		// closer to execute
		alarm(nxt_job->seconds - now);
	}
}

void onSuccess(Msg msg, List* jobs, int queue_id) {
	// msg_print(&msg);
	list_push_back(jobs, job_create(msg.t, msg.s));
	check_run(jobs, queue_id);
}

void schedule (List* jobs, int queue_id) {
	Msg msg;
	
	int virtual_id = N+1;
	while (true) {
		int res = msgrcv(queue_id, &msg, sizeof(Msg) - sizeof(long), virtual_id, 0);
	//	printf("cont: %d\n", cont);
//		if (!strcmp(msg.s, "finished")) {
		if (strstr (msg.s, "finished") != NULL) {
			cont++;
			strcat(traces, msg.s);
			strcat(traces, "->schedule.\n");
			strcpy(msg.s, traces);
			if ((cont == N && topology_type != TREE) || (cont == N - 1 && topology_type == TREE)) {
				S("...Job finished... ");
				printf("\n=> Traces\n%s\n", msg.s);
				cont = 0;
			} 

			if (cont >= N) {
				cont = 0;
			}

		} else {
			if (res < 0) {
				onError(jobs, queue_id);
			} else {
				onSuccess(msg, jobs, queue_id);
			}
		}
	}

	destroy(jobs, queue_id);
}
/**
 * @brief Creation of all management processes.
 * Return the process id's.
 * 
 */

void create_managers () {
	// Setup pids
	for (int i = 0; i < N; i++) {
		pids[i] = 0;
	}

	// Create the processes to manage
	for (int i = 0; i < N; i++) {
		pid = fork();

		if (pid == 0) {
			to_manage(i);
		} else {
			pids[i] = pid;
		}
	}
}
/**
 * @brief Set the up topology object
 * 
 * @param n 
 * @param v 
 */
void setup_topology (int n, char *v[]) {
	if (n != 2 || !try_cast_int(v[1], &topology_type)) {
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
}
/**
 * @brief Set the up alarm object
 * 
 */
void setup_alarm () {
	signal(SIGALRM, *dummy);
}
/**
 * @brief Executa o programa principal
 * 
 * @param argc 
 * @param argv 
 * @return int 
 */
int main (int argc, char *argv[]) {
	setup_alarm();
	setup_topology(argc, argv);

	int queue_id = queue_create(KEY);

	List *jobs = list_create();

	create_managers();

	schedule(jobs, queue_id);

	return 0;
}