#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <time.h>

#include "constants.h"

int type;
int oven_index, table_index;
int shm_d;
sem_t *semaphores[4];

void handle_sigint(int sig);
void prepare_pizza();
void put_pizza_into_oven();
void baking();
void move_pizza_form_oven_to_table();
void close_IPCs();

int main(int argc, char **argv){

	atexit(close_IPCs);
	signal(SIGINT, handle_sigint);
	srand(time(NULL) + clock());

	semaphores[0] = sem_open("/memory", O_RDWR);
	if (semaphores[0]==SEM_FAILED){
		perror("Can't open semaphore");
		exit(1);
	}
	semaphores[1] = sem_open("/oven", O_RDWR);
	if (semaphores[1]==SEM_FAILED){
		perror("Can't open semaphore");
		exit(1);
	}
	semaphores[2] = sem_open("/table_d", O_RDWR);
	if (semaphores[2]==SEM_FAILED){
		perror("Can't open semaphore");
		exit(1);
	}
	semaphores[3] = sem_open("/table_c", O_RDWR | O_CREAT, 0666, 0);
	if (semaphores[3]==SEM_FAILED){
		perror("Can't create semaphore");
		exit(1);
	}

	shm_d = shm_open("/mem", O_RDWR, 0666);
	if (shm_d==-1){
		perror("Can't open shared memory");
		exit(1);
	}
	

	while(1){
		prepare_pizza();
		put_pizza_into_oven();
		baking();
		move_pizza_form_oven_to_table();

	}

}

void close_IPCs(){

	for(int i=0; i<4; i++){
		sem_close(semaphores[i]);
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

	sem_wait(semaphores[1]);

	sem_wait(semaphores[0]);


	struct mem *memory = mmap(NULL, sizeof(struct mem), PROT_READ | PROT_WRITE, MAP_SHARED, shm_d, 0);
	for(int i=0; i<OVEN_SIZE; i++){
		if(memory->oven[i]!=1){
			oven_index=i;
			break;
		}
	}
	memory->oven[oven_index] = 1;
	memory->pizzas_in_oven++;
	printf("(%d, %ld) Dodałem pizze: %d. Liczba pizz w piecu: %d\n", getpid(), time(NULL), type, memory->pizzas_in_oven);
	munmap(memory, sizeof(struct mem));

	sem_post(semaphores[0]);

}


void baking(){
	sleep(4 + (rand() % 2));
}


void move_pizza_form_oven_to_table(){

	sem_wait(semaphores[3]);

	sem_post(semaphores[2]);
	
	sem_post(semaphores[1]);

	

	sem_wait(semaphores[0]);


	struct mem *memory = mmap(NULL, sizeof(struct mem), PROT_READ | PROT_WRITE, MAP_SHARED, shm_d, 0);


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
	munmap(memory, sizeof(struct mem));

	sem_post(semaphores[0]);



}
