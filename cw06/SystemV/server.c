#include <stdio.h>
#include <sys/msg.h>
#include <sys/ipc.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>

#include "constants.h"



int queue;
int customers_queues[MAX_CUSTOMERS];
int customers_friend[MAX_CUSTOMERS];
int customers=0;
int stop=0;

void handle_sigint(int sig);

void delete_queue(){
    msgctl(queue, IPC_RMID, NULL);
}

void handle_stop(struct msg *message);

void handle_disconnect(struct msg *message);

void handle_list(struct msg *message);

void handle_init(struct msg *message);

void handle_connect(struct msg *message);

int get_customer_id();


int main(int argc, char **argv){

    atexit(delete_queue);

    for(int i = 0; i < MAX_CUSTOMERS; i++){
        customers_friend[i] = -1;
        customers_queues[i] = -1;
    }

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handle_sigint;

    sigaction(SIGINT, &sa, NULL);


    key_t key;

    if ((key = ftok(getenv("HOME"), SERVER_ID)) == -1){
        perror("Can't generate key");
    }
    if ((queue = msgget(key, IPC_CREAT | IPC_EXCL | 0600)) == -1){
        perror("Can't create queue");
        exit(1);
    }


    struct msg message;

    while(1){


        if(msgrcv(queue, &message, sizeof(struct msg) - sizeof(long), -10, 0)==-1){
            perror("Can't receive message");
            continue;
        }


        switch (message.mtype){
            case STOP:{
                handle_stop(&message);
                break;
            }
            case DISCONNECT:{
                handle_disconnect(&message);
                break;
            }
            case LIST:{
                handle_list(&message);
                break;
            }
            case INIT:{
                handle_init(&message);
                break;
            }
            case CONNECT:{
                handle_connect(&message);
                break;
            }
            default:{
                printf("Incorrect type");
                break;
            }

        }

    }

    return 0;
}

void handle_sigint(int sig){

    stop = 1;
    if (customers == 0){
        exit(0);
    }


    for (int i = 0; i < MAX_CUSTOMERS; i++){
        if (customers_queues[i] != -1){
            kill(customers_queues[i], SIGUSR1);
        }
    }
}


void handle_stop(struct msg *message){
    int customer_id = message->id;

    int second_customer_id = customers_friend[customer_id];
    customers_friend[customer_id] = -1;
    if(second_customer_id != -1){
        customers_friend[second_customer_id] = -1;
    }
    customers_queues[customer_id] = -1;

    customers--;
    if (customers == 0 && stop == 1){
        exit(0);
    }

}

void handle_disconnect(struct msg *message){
    int customer_id = message->id;
    int second_customer_id = customers_friend[customer_id];

    customers_friend[customer_id] = -1;
    customers_friend[second_customer_id] = -1;
}

void handle_list(struct msg *message){
    
    int customer_id = message->id;
    struct msg mes;
    mes.mtype = MESSAGE;
    for (int i = 0; i < MAX_CUSTOMERS; i++){
        if (customers_queues[i] != -1){
            if(customers_friend[i]==-1){
                sprintf(mes.mtext, "Customer %d is available to connect\n", i);
                if (msgsnd(customers_queues[customer_id], &mes, sizeof(struct msg) - sizeof(long), 0)==-1){
			        perror("Can't send message");
			    }
            }else{
                sprintf(mes.mtext, "Customer %d is not available to connect\n", i);
                if (msgsnd(customers_queues[customer_id], &mes, sizeof(struct msg) - sizeof(long), 0)==-1){
			        perror("Can't send message");
			    }
            }
        }
    }

}

void handle_init(struct msg *message){
    int customer_id;
    if ((customer_id = get_customer_id())==-1){
        printf("Max number of customers");
        return;
    }	


    key_t key = strtol(message->mtext, NULL, 10);
    int customer_queue;

    if ((customer_queue = msgget(key, 0))==-1){
        perror("Can' open queue");
    }
    customers_queues[customer_id] = customer_queue;

    struct msg mes;
    mes.mtype = INIT;
    mes.id = customer_id;

    if (msgsnd(customers_queues[customer_id], &mes, sizeof(struct msg) - sizeof(long), 0)==-1){
        perror("Can't send message");
    }
    customers++;

}

void handle_connect(struct msg *message){
    int customer_id = message->id;
    int second_customer_id = strtol(message->mtext, NULL, 10);
    struct msg mes;
    mes.mtype = CONNECT;

    if (customer_id == second_customer_id){
        sprintf(mes.mtext, "%d", -1);
        if (msgsnd(customers_queues[customer_id], &mes, sizeof(struct msg) - sizeof(long), 0)==-1){
            perror("Can't send message");
        }
        return;
    }

    if(customers_queues[second_customer_id] != -1 && customers_friend[second_customer_id]==-1){
        customers_friend[customer_id] = second_customer_id;
        customers_friend[second_customer_id] = customer_id;


        sprintf(mes.mtext, "%d", customers_queues[second_customer_id]);

        if (msgsnd(customers_queues[customer_id], &mes, sizeof(struct msg) - sizeof(long), 0)==-1){
            perror("Can't send message");
        }

        sprintf(mes.mtext, "%d", customers_queues[customer_id]);

        if (msgsnd(customers_queues[second_customer_id], &mes, sizeof(struct msg) - sizeof(long), 0)==-1){
            perror("Can't send message");
        }

    }else{
        sprintf(mes.mtext, "%d", -1);
        if (msgsnd(customers_queues[customer_id], &mes, sizeof(struct msg) - sizeof(long), 0)==-1){
            perror("Can't send message");
        }
    }

}

int get_customer_id(){
    for(int i = 0; i < MAX_CUSTOMERS; i++){
        if (customers_queues[i]==-1){
            return i;
        }
    }
    return -1;
}