#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

#include "constants.h"

pid_t pids[CHEFS+DELIVERERS];
int sem_id, shm_id;


void handle_sigint(int sig);
void create_semaphore();
void create_shared_memory();
void start_simulation();
void remove_IPCs();

union semun {
   int              val;    /* Value for SETVAL */
   struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
   unsigned short  *array;  /* Array for GETALL, SETALL */
   struct seminfo  *__buf;  /* Buffer for IPC_INFO
                               (Linux-specific) */
};

int main(int argc, char **argv){
	
	atexit(remove_IPCs);
	signal(SIGINT, handle_sigint);
	create_semaphore();
	create_shared_memory();
	start_simulation();
	return 0;

}

void handle_sigint(int sig){

	for(int i=0; i<CHEFS+DELIVERERS; i++){
		kill(pids[i], SIGINT);
	}
	exit(0);
}


void create_semaphore(){
	key_t sem_key = ftok(getenv("HOME"), SEM_KEY_ID);
	sem_id = semget(sem_key, 4, IPC_CREAT | 0666);
	if (sem_id==-1){
		perror("Can't create semaphore");
		exit(1);
	}
	union semun arg;
	arg.val = 1;

	semctl(sem_id, 0, SETVAL, arg);
	arg.val = 0;
	semctl(sem_id, 2, SETVAL, arg);

	arg.val = OVEN_SIZE;
	semctl(sem_id, 1, SETVAL, arg);
	
	arg.val = TABLE_SIZE;
	semctl(sem_id, 3, SETVAL, arg);


}


void create_shared_memory(){
	key_t shm_key = ftok(getenv("HOME"), SHM_KEY_ID);
	shm_id = shmget(shm_key, sizeof(struct mem), IPC_CREAT | 0666);
	if (shm_id==-1){
		perror("Can't create shared memory");
		exit(1);
	}

	struct mem *memory = shmat(shm_id, NULL, 0);
	for(int i=0; i<OVEN_SIZE; i++){
		memory->oven[i] = 0;

	}
	for(int i=0; i<TABLE_SIZE; i++){
		memory->table[i] = 0;
	}
	memory->pizzas_in_oven = 0;
	memory->pizzas_on_table = 0;

	shmdt(memory);

}


void start_simulation(){

	for(int i=0; i<CHEFS; i++){
		pid_t pid;
		if((pid=fork())==0){
			execlp("./chef", "./chef", NULL);
		}
		pids[i]=pid;
	}

	for(int i=CHEFS; i<CHEFS+DELIVERERS; i++){
		pid_t pid;
		if((pid=fork())==0){
			execlp("./deliverer", "./deliverer", NULL);
		}
		pids[i]=pid;
	}

	for(int i=0; i<CHEFS+DELIVERERS; i++){
		wait(NULL);
	}



}
void remove_IPCs(){
	semctl(sem_id, 0, IPC_RMID, NULL);
	shmctl(shm_id, IPC_RMID, NULL);
}