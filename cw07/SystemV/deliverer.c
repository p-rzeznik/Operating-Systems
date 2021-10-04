#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>

#include "constants.h"

int type;
int table_index;
int sem_id, shm_id;


void handle_sigint(int sig);
void get_pizza();
void deliver();


int main(int argc, char **argv){

	signal(SIGINT, handle_sigint);

	key_t sem_key = ftok(getenv("HOME"), SEM_KEY_ID);
	sem_id = semget(sem_key, 0, 0);
	if (sem_id==-1){
		perror("Can't get semaphore");
		exit(1);
	}

	key_t shm_key = ftok(getenv("HOME"), SHM_KEY_ID);
	shm_id = shmget(shm_key, 0, 0);
	if (shm_id==-1){
		perror("Can't get to shared memory");
		exit(1);
	}
	// printf("(%d, %ld)\n", getpid(), time(NULL));

    while(1){
        
        get_pizza();
        deliver();

    }

}

void handle_sigint(int sig){
	exit(0);
}
void get_pizza(){

	struct sembuf *op1 = calloc(2, sizeof(struct sembuf));
	op1[0].sem_num = 2;
	op1[0].sem_op = -1;
	op1[0].sem_flg = 0;
	op1[1].sem_num = 3;
	op1[1].sem_op = 1;
	op1[1].sem_flg = 0;

	semop(sem_id, op1, 2);

	free(op1);

	struct sembuf *op2 = calloc(1, sizeof(struct sembuf));
	op2[0].sem_num = 0;
	op2[0].sem_op = -1;
	op2[0].sem_flg = 0;
	
	semop(sem_id, op2, 1);


	struct mem *memory = shmat(shm_id, NULL, 0);



	for(int i=0; i<TABLE_SIZE; i++){
		if(memory->table[i]!=0){
			table_index=i;
			break;
		}
	}
	type = memory->table[table_index];
	memory->pizzas_on_table--;

	printf("(%d, %ld) Pobieram pizze: %d. Liczba pizz na stole: %d\n", getpid(), time(NULL), memory->table[table_index], memory->pizzas_on_table);
	
	memory->table[table_index] = 0;

	shmdt(memory);


	op2[0].sem_num = 0;
	op2[0].sem_op = 1;
	op2[0].sem_flg = 0;
	
	semop(sem_id, op2, 1);

	free(op2);


	



}


void deliver(){
	sleep(4 + (rand() % 2));
	printf("(%d, %ld) Dostarczam pizze: %d.\n", getpid(), time(NULL), type);
	sleep(4 + (rand() % 2));
}