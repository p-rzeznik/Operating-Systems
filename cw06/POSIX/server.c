#include <stdio.h>
#include <fcntl.h>
#include <mqueue.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>

#include "constants.h"



mqd_t qd;
char *name;

int customers_qds[MAX_CUSTOMERS];
int customers_pids[MAX_CUSTOMERS];
int customers_friend[MAX_CUSTOMERS];

int customers=0;
int stop=0;

void handle_sigint(int sig);

void at_exit();

void handle_stop(char *message);

void handle_disconnect(char *message);

void handle_list(char *message);

void handle_init(char *message);

void handle_connect(char *message);

int get_customer_id();


int main(int argc, char **argv){


    atexit(at_exit);

    for(int i = 0; i < MAX_CUSTOMERS; i++){
        customers_friend[i] = -1;
        customers_qds[i] = -1;
        customers_pids[i] = -1;
    }

    struct sigaction sa;
    sigemptyset(&sa.sa_mask);
    sa.sa_flags = 0;
    sa.sa_handler = handle_sigint;

    sigaction(SIGINT, &sa, NULL);

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MESSAGE_SIZE - 1;
    attr.mq_curmsgs = 0;

    if ((qd = mq_open(SERVER_NAME, O_RDONLY | O_CREAT | O_EXCL, 0666, &attr))==-1){
        perror("Can't create queue");
        exit(1);
    }


    char message[MAX_MESSAGE_SIZE];

    while(1){

        uint type;

        if(mq_receive(qd, message, MAX_MESSAGE_SIZE, &type)==-1){
            perror("Can't receive message");
            continue;
        }

        printf("%s\n", message);

        switch (type){
            case STOP:{
                printf("r STOP\n");
                handle_stop(message);
                break;
            }
            case DISCONNECT:{
                printf("r DISCONNECT\n");
                handle_disconnect(message);
                break;
            }
            case LIST:{
                printf("r LIST\n");
                handle_list(message);
                break;
            }
            case INIT:{
                printf("r INIT\n");
                handle_init(message);
                break;
            }
            case CONNECT:{
                printf("r CONNECT\n");
                handle_connect(message);
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

void at_exit(){
    if(customers>0){
    char mes[MAX_MESSAGE_SIZE];
    sprintf(mes, "%d", SERVER_STOP);

    for(int i = 0; i < MAX_CUSTOMERS; i++){
        if (customers_qds[i]!=-1){
            mq_send(customers_qds[i], mes, strlen(mes), SERVER_STOP);
        }
    }
    }
    mq_close(qd);
    mq_unlink(SERVER_NAME); 

}

void handle_sigint(int sig){
    exit(0);
}


void handle_stop(char *message){
    int type, customer_id;
    sscanf(message, "%d %d", &type, &customer_id);

    int second_customer_id = customers_friend[customer_id];
    customers_friend[customer_id] = -1;

    if(second_customer_id != -1){
        customers_friend[second_customer_id] = -1;
    }
    mq_close(customers_qds[customer_id]);

    customers_qds[customer_id] = -1;

    customers--;
    if (customers == 0 && stop == 1){
        exit(0);
    }

}

void handle_disconnect(char *message){
    int type, customer_id;
    sscanf(message, "%d %d", &type, &customer_id);

    int second_customer_id = customers_friend[customer_id];

    customers_friend[customer_id] = -1;
    customers_friend[second_customer_id] = -1;
}


void handle_list(char *message){
    int type, customer_id;
    sscanf(message, "%d %d", &type, &customer_id);

    char mes[MAX_MESSAGE_SIZE];
    for (int i = 0; i < MAX_CUSTOMERS; i++){
        if (customers_qds[i] != -1){
            char tmp[MAX_MESSAGE_SIZE];
            if(customers_friend[i]==-1){
                sprintf(tmp, "%d Customer %d is available to connect\n", MESSAGE, i);
                strcat(mes, tmp);

            }else{
                sprintf(tmp, "%d Customer %d is not available to connect\n", MESSAGE, i);
                strcat(mes, tmp);
            }
        }
    }

    if (mq_send(customers_qds[customer_id], mes, strlen(mes), MESSAGE)==-1){
        perror("Can't send message");
    }

}

void handle_init(char *message){
    int customer_id;
    if ((customer_id = get_customer_id())==-1){
        printf("Max number of customers");
        return;
    }   


    int type;
    char name[MAX_MESSAGE_SIZE];

    sscanf(message, "%d %d", &type, &customers_pids[customer_id]);
    sprintf(name, "/q-%d", customers_pids[customer_id]);
    printf("%d\n", customers_pids[customer_id]);

    int customer_queue;
    if ((customer_queue = mq_open(name, O_WRONLY))==-1){
        perror("Can' open queue");
    }

    customers_qds[customer_id] = customer_queue;

    char mes[MAX_MESSAGE_SIZE];
    sprintf(mes, "%d %d", INIT, customer_id);


    if (mq_send(customers_qds[customer_id], mes, strlen(mes), INIT)==-1){
        perror("Can't send message");
    }

    customers++;

}

void handle_connect(char *message){
    int type, customer_id, second_customer_id;
    sscanf(message, "%d %d %d", &type, &customer_id, &second_customer_id);

    char mes[MAX_MESSAGE_SIZE];

    if (customer_id == second_customer_id){
        sprintf(mes, "%d %d", CONNECT, -1);
        if (mq_send(customers_qds[customer_id], mes, strlen(mes), CONNECT)==-1){
            perror("Can't send message");
        }
        return;
    }

    if(customers_qds[second_customer_id] != -1 && customers_friend[second_customer_id]==-1){
        customers_friend[customer_id] = second_customer_id;
        customers_friend[second_customer_id] = customer_id;


        sprintf(mes, "%d %d", CONNECT, customers_pids[second_customer_id]);
        printf("pid1: %d, pid2: %d", customers_pids[customer_id], customers_pids[customer_id]);
        if (mq_send(customers_qds[customer_id], mes, strlen(mes), CONNECT)==-1){
            perror("Can't send message");
        }

        sprintf(mes, "%d %d", CONNECT, customers_pids[customer_id]);

        if (mq_send(customers_qds[second_customer_id], mes, strlen(mes), CONNECT)==-1){
            perror("Can't send message");
        }

    }else{
        sprintf(mes, "%d %d", CONNECT, -1);
        if (mq_send(customers_qds[customer_id], mes, strlen(mes), CONNECT)==-1){
            perror("Can't send message");
        }
    }

}

int get_customer_id(){
    for(int i = 0; i < MAX_CUSTOMERS; i++){
        if (customers_qds[i]==-1){
            return i;
        }
    }
    return -1;
}