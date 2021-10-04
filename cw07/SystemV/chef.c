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
int oven_index, table_index;
int sem_id, shm_id;

void handle_sigint(int sig);
void prepare_pizza();
void put_pizza_into_oven();
void baking();
void move_pizza_form_oven_to_table();

int main(int argc, char **argv){

	signal(SIGINT, handle_sigint);
	srand(time(NULL) + clock());
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

	while(1){
		prepare_pizza();
		put_pizza_into_oven();
		baking();
		move_pizza_form_oven_to_table();

	}

}



void handle_sigint(int sig){
    exit(0);
}

void prepare_pizza(){
	type = rand() % 10;
	
	printf("(%d, %ld) Przygotowuje pizze: %d\n", getpid(), time(NULL), type);

	sleep(1 + (rand() % 2));
}


void put_pizza_into_oven(){

	struct sembuf *op1 = calloc(1, sizeof(struct sembuf));
	op1[0].sem_num = 1;
	op1[0].sem_op = -1;
	op1[0].sem_flg = 0;

	semop(sem_id, op1, 1);


	free(op1);


	struct sembuf *op2 = calloc(1, sizeof(struct sembuf));
	op2[0].sem_num = 0;
	op2[0].sem_op = -1;
	op2[0].sem_flg = 0;
	
	semop(sem_id, op2, 1);

	free(op2);

	struct mem *memory = shmat(shm_id, NULL, 0);
	for(int i=0; i<OVEN_SIZE; i++){
		if(memory->oven[i]!=1){
			oven_index=i;
			break;
		}
	}
	memory->oven[oven_index] = 1;
	memory->pizzas_in_oven++;
	printf("(%d, %ld) Dodałem pizze: %d. Liczba pizz w piecu: %d\n", getpid(), time(NULL), type, memory->pizzas_in_oven);
	shmdt(memory);


	struct sembuf *op3 = calloc(1, sizeof(struct sembuf));

	op3[0].sem_num = 0;
	op3[0].sem_op = 1;
	op3[0].sem_flg = 0;
	
	semop(sem_id, op3, 1);

	free(op3);


}


void baking(){
	sleep(4 + (rand() % 2));
}


void move_pizza_form_oven_to_table(){

	struct sembuf *op1 = calloc(3, sizeof(struct sembuf));
	op1[0].sem_num = 3;
	op1[0].sem_op = -1;
	op1[0].sem_flg = 0;

	op1[1].sem_num = 1;
	op1[1].sem_op = 1;
	op1[1].sem_flg = 0;

	op1[2].sem_num = 2;
	op1[2].sem_op = 1;
	op1[2].sem_flg = 0;
	
	semop(sem_id, op1, 3);

	free(op1);

	struct sembuf *op2 = calloc(1, sizeof(struct sembuf));
	op2[0].sem_num = 0;
	op2[0].sem_op = -1;
	op2[0].sem_flg = 0;
	
	semop(sem_id, op2, 1);


	struct mem *memory = shmat(shm_id, NULL, 0);


	memory->oven[oven_index] = 0;
	memory->pizzas_in_oven--;

	for(int i=0; i<TABLE_SIZE; i++){
		if(memory->table[i]==0){
			table_index=i;
			break;
		}
	}
	memory->table[table_index] = type;
	memory->pizzas_on_table++;


	printf("(%d, %ld) Wyjąłem pizze: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d\n", getpid(), time(NULL), type, memory->pizzas_in_oven, memory->pizzas_on_table);
	shmdt(memory);


	op2[0].sem_num = 0;
	op2[0].sem_op = 1;
	op2[0].sem_flg = 0;
	
	semop(sem_id, op2, 1);

	free(op2);


}
