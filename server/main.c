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

int pids[N+1];
int structure[N+1];
int vis[N+1];

int cont = 0;
int queue_id;

char traces[1000];

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

void ft_down (int idx, Msg msg) {
	if ((2 * idx + 1) <= N) {
		msg.type = 2 * idx;
		msgsnd(queue_id, &msg,  sizeof(Msg) - sizeof(long), IPC_NOWAIT);
		msg.type = 2 * idx + 1;
		msgsnd(queue_id, &msg,  sizeof(Msg) - sizeof(long), IPC_NOWAIT);
	}
}

void hc_down (int idx, Msg msg) {

}

void tr_down (int idx, Msg msg) {

	if (idx < M) { // first send to node 2, 3 and 4
		msg.type = idx + 1;
		msgsnd(queue_id, &msg,  sizeof(Msg) - sizeof(long), IPC_NOWAIT);
	}

	if (idx <= N - M) { // nodes 1, 2, 3 and 4 sendo to above nodes
		msg.type = idx + M;
		msgsnd(queue_id, &msg,  sizeof(Msg) - sizeof(long), IPC_NOWAIT);
	}
}

void ft_up (int idx, Msg msg) {

	if (idx > 1) {
		msg.type = idx / 2;
		msgsnd(queue_id, &msg,  sizeof(Msg) - sizeof(long), IPC_NOWAIT);
	} else {
		msg.type = N + 1;
		msgsnd(queue_id, &msg, sizeof(Msg) - sizeof(long), IPC_NOWAIT);
	}
}

void hc_up (int idx, Msg msg) {

}

/* Structure: root node 1
13 14 15 16  
9  10 11 12
5  6  7  8
1  2  3  4   // First line
*/
void tr_up (int idx, Msg msg) {

	int up;
	if (idx > 1) { // if isn't the root node

		if(idx > M * 3) {// if top line, send to first line
			up = (idx + M) % N; 
		} else if (idx % M == 0) { // if last right column, send to first column
			up = (idx - M + 1); 
		} else if (idx % M == 1) { // if first column, send to the below node
			up = idx - M;
		} else { // if first line or mid, go to the previous left node
			up = idx - 1;
		}

	}	else { // if root node, send to scheduler
		up = N + 1;
	}
		msg.type = up;
		msgsnd(queue_id, &msg, sizeof(Msg) - sizeof(long), IPC_NOWAIT);
}

void br_down(int idx, Msg msg) {
	switch (topology_type) {
		case TREE:
			ft_down(idx, msg);
			break;
		case HYPERCUBE:
			hc_down(idx, msg);
			break;
		case TORUS:
			tr_down(idx, msg);
			break;
	}
}

void br_up(int idx, Msg msg) {
	switch (topology_type) {
		case TREE:
			ft_up(idx, msg);
			break;
		case HYPERCUBE:
			hc_up(idx, msg);
			break;
		case TORUS:
			tr_up(idx, msg);
			break;
	}
}

void itoa(int i, char b[]){
    char const digit[] = "0123456789";
    char* p = b;
    if(i<0){
        *p++ = '-';
        i *= -1;
    }
    int shifter = i;
    do{ //Move to where representation ends
        ++p;
        shifter = shifter/10;
    }while(shifter);
    *p = '\0';
    do{ //Move back, inserting digits as u go
        *--p = digit[i%10];
        i = i/10;
    }while(i);
}

void mng_on_success(int idx, Msg msg) {

//	if (strcmp(msg.s, "finished")) { // if ins't a new message
	if (strstr (msg.s, "finished") == NULL) {
		br_down(idx, msg);
		msg = execute_job(idx, msg.s);
	}

		char temp[12] = "\0";
		itoa(idx, temp);
		strcat(msg.s, "->");
		strcat(msg.s, temp);

	br_up(idx, msg);
}

void mng_on_error() {
	E("Failed to receive message");
	printf("A process was killed...\n");
	fflush(stdout);
	exit(1);
}

void receive_msg(int idx) {
	Msg msg;

	while (true) {
		int res = msgrcv(queue_id, &msg, sizeof(Msg) - sizeof(long), idx, 0);

		if (res < 0) {
			mng_on_error();
		} else {
			mng_on_success(idx, msg);
		}
	}
}

void to_manage(int idx) {
	queue_id = queue_retrieve(KEY);

	receive_msg(idx);
	exit(1);
}

// -------------- End of Managers

void destroy(List *jobs, int queue_id) {
  list_destroy(jobs);

	struct msqid_ds msg;
  msgctl(queue_id, IPC_RMID, &msg);
}

void remove_next_job (List* jobs, int id) {
  time_t t = time(NULL);
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
	time_t t = time(NULL);
	Job *job = NULL;
	Node *curr = jobs->begin;

	while (curr != NULL) {
		Job *aux = (Job*) curr->value;

		if (job == NULL || job->seconds > aux->seconds) {
			job = aux;
		}

		curr = curr->nxt;
	}

	return (job != NULL && job->seconds <= t) ? job : NULL;
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

/*!
 *  \brief Creation of all management processes.
 *  \return the process id's.
 */
void create_managers () {
	// Setup pids
	int total = topology_type == TREE ? N - 1 : N;

	for (int i = 1; i <= total; i++) {
		pids[i] = 0;
	}

	// Create the processes to manage
	for (int i = 1; i <= total; i++) {
		pid = fork();

		if (pid == 0) {
			to_manage(i);
		} else {
			pids[i] = pid;
		}
	}
}

void setup_topology (int n, char *v[]) {
	if (n != 2 || !try_cast_int(v[1], &topology_type)) {
		E("Not a valid topology");
		exit(1);
	}

	switch (topology_type) {
		case TREE:
			ft_make(structure, vis);
			break;
		case HYPERCUBE:
			hc_make(structure, vis);
			break;
		case TORUS:
			tr_make(structure, vis);
			break;
		default:
			E("Wrong topology!");
			exit(1);
	}

	S("Topology set");
}

void setup_alarm () {
	signal(SIGALRM, *dummy);
}

int main (int argc, char *argv[]) {
	setup_alarm();
	setup_topology(argc, argv);

	int queue_id = queue_create(KEY);

	List *jobs = list_create();

	create_managers();

	schedule(jobs, queue_id);

	return 0;
}
