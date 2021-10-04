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
int table_index;
int shm_d;
sem_t *semaphores[4];


void handle_sigint(int sig);
void get_pizza();
void deliver();
void close_IPCs();


int main(int argc, char **argv){

	atexit(close_IPCs);

	signal(SIGINT, handle_sigint);

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
	semaphores[3] = sem_open("/table_c", O_RDWR);
	if (semaphores[3]==SEM_FAILED){
		perror("Can't create semaphore");
		exit(1);
	}

	shm_d = shm_open("/mem", O_RDWR, 0666);
	if (shm_d==-1){
		perror("Can't open shared memory");
		exit(1);
	}
	
	// printf("(%d, %ld)\n", getpid(), time(NULL));

    while(1){
        
        get_pizza();
        deliver();

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
void get_pizza(){

	sem_wait(semaphores[2]);


	sem_post(semaphores[3]);


	sem_wait(semaphores[0]);


	struct mem *memory = mmap(NULL, sizeof(struct mem), PROT_READ | PROT_WRITE, MAP_SHARED, shm_d, 0);



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

	munmap(memory, sizeof(struct mem));


	sem_post(semaphores[0]);

}


void deliver(){
	sleep(4 + (rand() % 2));
	printf("(%d, %ld) Dostarczam pizze: %d.\n", getpid(), time(NULL), type);
	sleep(4 + (rand() % 2));
}