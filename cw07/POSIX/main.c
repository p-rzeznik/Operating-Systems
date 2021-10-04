#include <sys/types.h>
#include <sys/wait.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <fcntl.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <stdio.h>

#include "constants.h"

pid_t pids[CHEFS+DELIVERERS];
int  shm_d;
sem_t *semaphores[4];

void handle_sigint(int sig);
void create_semaphore();
void create_shared_memory();
void start_simulation();
void remove_IPCs();


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
	semaphores[0] = sem_open("/memory", O_RDWR | O_CREAT, 0666, 1);
	if (semaphores[0]==SEM_FAILED){
		perror("Can't create semaphore");
		exit(1);
	}
	semaphores[1] = sem_open("/oven", O_RDWR | O_CREAT, 0666, OVEN_SIZE);
	if (semaphores[1]==SEM_FAILED){
		perror("Can't create semaphore");
		exit(1);
	}
	semaphores[2] = sem_open("/table_d", O_RDWR | O_CREAT, 0666, 0);
	if (semaphores[2]==SEM_FAILED){
		perror("Can't create semaphore");
		exit(1);
	}
	semaphores[3] = sem_open("/table_c", O_RDWR | O_CREAT, 0666, TABLE_SIZE);
	if (semaphores[3]==SEM_FAILED){
		perror("Can't create semaphore");
		exit(1);
	}

	for(int i=0; i<4; i++){
		sem_close(semaphores[i]);
	}

}


void create_shared_memory(){


	shm_d = shm_open("/mem", O_RDWR | O_CREAT, 0666);
	if (shm_d==-1){
		perror("Can't create shared memory");
		exit(1);
	}
	if(ftruncate(shm_d, sizeof(struct mem))){
		perror("Can't create shared memory");
		exit(1);
	}


	struct mem *memory = mmap(NULL, sizeof(struct mem), PROT_WRITE, MAP_SHARED, shm_d, 0);
	for(int i=0; i<OVEN_SIZE; i++){
		memory->oven[i] = 0;

	}
	for(int i=0; i<TABLE_SIZE; i++){
		memory->table[i] = 0;
	}
	memory->pizzas_in_oven = 0;
	memory->pizzas_on_table = 0;

	munmap(memory, sizeof(struct mem));

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
	sem_unlink("/memory");
	sem_unlink("/oven");
	sem_unlink("/table_c");
	sem_unlink("/table_d");
	
	shm_unlink("/mem");
}