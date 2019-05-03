#include "semaphore.h"

struct semaphore {
    int id;
    struct sembuf def[2];
};

void sem_op (Semaphore* sem, int idx, int num, int op, int flag) {
  sem->def[idx].sem_num = num;
  sem->def[idx].sem_op = op;
  sem->def[idx].sem_flg = flag;
}

void P (Semaphore* sem) {
    sem_op(sem, 0, 0, 0, 0);
    sem_op(sem, 1, 0, 1, 0);

    int res = semop(sem->id, sem->def, 2);

    if (res < 0) {
        printf("erro no p=%d\n", errno);
    }
}

void V (Semaphore* sem) {
    sem_op(sem, 0, 0, -1, 0);

    int res = semop(sem->id, sem->def, 2);

    if (res < 0) {
        printf("erro no p=%d\n", errno);
    }
}