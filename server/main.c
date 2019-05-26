#include "util.h"

#include "job.h"
#include "list.h"
#include "message.h"
#include "manager.h"

#include "tree.h"
#include "hypercube.h"
#include "torus.h"

// Data
int pid;
int topology_type;

int pids[N+1];
int structure[N+1];
int vis[N+1];

void destroy(List *jobs, int msg_id) {
  list_destroy(jobs);

	struct msqid_ds msg;
  msgctl(msg_id, IPC_RMID, &msg);
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

void run (Job* job, int msg_id) {
	// Send message to node 0 to start execution
	Msg msg;

	msg.t = 0;
	msg.type = 0; // To send message to node 0
	strcpy(msg.s, job->filename);

	msgsnd(msg_id, &msg,  sizeof(Msg), 0);
	// TODO: wait for message sent by node 0
}

void onError(List* jobs, int msg_id) {
	if (errno == EINTR && alarm(0) == 0) {  // https://stackoverflow.com/questions/41474299/checking-if-errno-eintr-what-does-it-mean
		// Checked if the interruption error was caused 
		// by an alarm (deactivates the alarm in the process)

		Job *nxt_job = next_job(jobs);
		run(nxt_job, msg_id);
	} else {
		// The interruption error was not caused by the alarm or 
		// failed to receive (no interruption error)

		E("Failed to receive message");
	}
}

void check_run (List* jobs, int msg_id) {
	Job *nxt_job = next_job(jobs);
	time_t now = time(NULL);
	printf("seconds %d\n", nxt_job->seconds);
	printf("now: %d\n", now);
	if (nxt_job->seconds <= now) {
		// Deactivate the alarm and execute if it was
		// to execute now the file

		alarm(0);
		remove_next_job(jobs, nxt_job->id);
		run(nxt_job, msg_id);
		check_run(jobs, msg_id);
	} else {
		// Modify the alarm for the new job, since it is 
		// closer to execute

		alarm(nxt_job->seconds - now);
	}
}

void onSuccess(Msg msg, List* jobs, int msg_id) {
	msg_print(&msg);
	list_push_back(jobs, job_create(msg.t, msg.s));
	check_run(jobs, msg_id);
}

void schedule (List* jobs) {
	int msg_id = msg_create(KEY);
	Msg msg;

	while (true) {
		int virtual_id = N+1;
		int res = msgrcv(msg_id, &msg,  sizeof(Msg) /* - sizeof(long) */, virtual_id, 0);

		if (res < 0) {
			onError(jobs, msg_id);
		} else {
			onSuccess(msg, jobs, msg_id);
		}
	}

	destroy(jobs, msg_id);
}

/*!
 *  \brief Creation of all management processes.
 *  \return the process id's.
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
			to_manage(i, TREE);
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

	List *jobs = list_create();

	create_managers();

	schedule(jobs);

	return 0;
}
