#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>
#include <signal.h>

#define REINDEER 9
#define ELFS 10

pthread_t elfs_nearby_house[3];
int elfs_nearby_house_count = 0;
pthread_t elfs_tid[ELFS];
pthread_t reindeer_tid[REINDEER];
pthread_t santa_tid;
int sleeping = 0;
int reindeer_on_north_pole = 0;

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t sleep_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t reindeer_cond = PTHREAD_COND_INITIALIZER;
pthread_cond_t elf_cond = PTHREAD_COND_INITIALIZER;

void *santa();
void *reindeer();
void *elf();
int get_idx();
void handle_sigint(int sig);
void handle_sigusr1(int sig);

int main(int argc, char **argv){

    srand(time(NULL));
    signal(SIGINT, handle_sigint);
    signal(SIGUSR1, handle_sigusr1);

    for (int i = 0; i <3;i++){
		elfs_nearby_house[i]=-1;
	}
    pthread_create(&santa_tid, NULL, santa, NULL);
    for(int i = 0; i < REINDEER; i++){
    	pthread_create(&reindeer_tid[i], NULL, reindeer, NULL);
    } 
    for(int i = 0; i < ELFS; i++){
    	pthread_create(&elfs_tid[i], NULL, elf, NULL);
    } 


    pthread_join(santa_tid, NULL);
    for(int i = 0; i < REINDEER; i++){
    	pthread_join(reindeer_tid[i], NULL);
    } 
    for(int i = 0; i < ELFS; i++){
    	pthread_join(elfs_tid[i], NULL);
    } 
    return 0;

}


void *santa(){

	while(1){
		pthread_mutex_lock(&mutex);
		if(reindeer_on_north_pole!=REINDEER && elfs_nearby_house_count!=3){
			printf("Mikołaj: Idę spać.\n");
			sleeping = 1;
			pthread_cond_wait(&sleep_cond, &mutex);
			sleeping = 0;
		}
		printf("Mikołaj: Budzę się.\n");

		if(reindeer_on_north_pole==REINDEER){
			printf("Mikołaj: Dostarczam zabawki.\n");
			sleep(rand()%2+2);
			reindeer_on_north_pole = 0;
			pthread_cond_broadcast(&reindeer_cond);
		}else{
			printf("Mikołaj: Rozwiązuję problemy elfów %ld, %ld, %ld ID.\n", elfs_nearby_house[0], elfs_nearby_house[1], elfs_nearby_house[2]);
			for (int i = 0; i <3; i++){
				pthread_kill(elfs_nearby_house[i], SIGUSR1);
				elfs_nearby_house[i] = -1;
			}
			sleep(rand()%1+1);
			elfs_nearby_house_count = 0;
			pthread_cond_broadcast(&elf_cond);
		}
		pthread_mutex_unlock(&mutex);

	}
	return (void*)0;
}

void *reindeer(){
	while(1){
		sleep(rand()%5+5);
		pthread_mutex_lock(&mutex);
		printf("Renifer: czeka %d reniferów na Mikołaja.\n", ++reindeer_on_north_pole);
		if(reindeer_on_north_pole!=REINDEER){
			pthread_cond_wait(&reindeer_cond, &mutex);

		}else{
			printf("Renifer: Wybudzam Mikołaja, %ld ID\n", pthread_self());
			pthread_cond_broadcast(&sleep_cond);
			if(reindeer_on_north_pole!=0){
				pthread_cond_wait(&reindeer_cond, &mutex);
			}
		}
		
		pthread_mutex_unlock(&mutex);

	}
	return (void*)0;
}
void *elf(){
	while(1){
		sleep(rand()%3+2);
		pthread_mutex_lock(&mutex);
		if(elfs_nearby_house_count==3){
			printf("Elf: czekam na powrót elfów %ld ID\n", pthread_self());
			pthread_cond_wait(&elf_cond, &mutex);

		}else{
			elfs_nearby_house_count++;
			printf("Elf: czeka %d elfów na Mikołaja.\n", elfs_nearby_house_count);
			elfs_nearby_house[get_idx()] = pthread_self();
			if(elfs_nearby_house_count!=3){
				pthread_cond_wait(&elf_cond, &mutex);
			}else{
				printf("Elf: Wybudzam Mikołaja, %ld ID\n", pthread_self());
				pthread_cond_broadcast(&sleep_cond);
				if(elfs_nearby_house_count!=0){
					pthread_cond_wait(&elf_cond, &mutex);
				}

			}
		}
		
		pthread_mutex_unlock(&mutex);

	}
	return (void*)0;
}
int get_idx(){
	for (int i = 0; i <3;i++){
		if(elfs_nearby_house[i]==-1){
			return i;
		}
	}
	return -1;
}

void handle_sigint(int sig){

	exit(0);
}
void handle_sigusr1(int sig){
	printf("Elf: Mikołaj rozwiązuje problem, %ld ID\n", pthread_self());
}