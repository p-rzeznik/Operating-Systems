#include <stdio.h>
#include <fcntl.h>
#include <mqueue.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "constants.h"

int server_queue;
int my_queue;
char my_name[MAX_MESSAGE_SIZE];
int friends_queue = -1;
int id;


void before_exit();
void handle_sigint(int sig);
void handle_stop();
void handle_list();
void handle_connect(char *text);
void handle_disconnect();
void send_message(char *text);
void handle_friend_connect(char *message);
void handle_friend_disconnect(char *message);
void handle_message(char *message);
void recevie_message(int sig);

int main(int argc, char **argv){

    atexit(before_exit);



    if ((server_queue = mq_open(SERVER_NAME, O_WRONLY)) == -1){
        perror("Can't open server queue");
        exit(1);
    }
    sprintf(my_name, "/q-%d", getpid());

    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MESSAGE_SIZE - 1;
    attr.mq_curmsgs = 0;

    if ((my_queue = mq_open(my_name, O_RDONLY | O_CREAT | O_EXCL, 0666, &attr)) == -1){
        perror("Can't create queue");
        exit(1);
    }


    char mes[MAX_MESSAGE_SIZE];

    sprintf(mes, "%d %d", INIT, getpid());


    if(mq_send(server_queue, mes, strlen(mes), INIT)==-1){
        perror("Can't send message");
        exit(1);
    }

    char mes_received[MAX_MESSAGE_SIZE];
    uint prior;
    if(mq_receive(my_queue, mes_received, MAX_MESSAGE_SIZE, &prior)==-1){
        perror("Can't receive message");
        exit(1);
    }

    int type;
    sscanf(mes_received, "%d %d", &type, &id);

    printf("Your ID: %d\n",id);

    struct sigaction sa1;
    sigemptyset(&sa1.sa_mask);
    sa1.sa_flags = 0;
    sa1.sa_handler = handle_sigint;
    sigaction(SIGINT, &sa1, NULL);

    signal(SIGUSR1, recevie_message);


    struct sigevent sig_ev;
    sig_ev.sigev_notify = SIGEV_SIGNAL;
    sig_ev.sigev_signo = SIGUSR1;
    mq_notify(my_queue, &sig_ev);


    while(1){

        char text[1024];
        char text_copy[1024];

        fgets(text, 1024, stdin);

        strcpy(text_copy, text);

        char *text_tmp = text_copy;

        char *cmd = strtok_r(text_copy, " \n", &text_tmp);

        if(strcmp(cmd, "STOP")==0){
            handle_stop();
            printf("h STOP\n");
        }else if (strcmp(cmd, "LIST")==0){
            handle_list();
            printf("h LIST\n");
        }else if(strcmp(cmd, "CONNECT")==0){
            handle_connect(text);
            printf("h CONNECT\n");
        }else if(strcmp(cmd, "DISCONNECT")==0){
            handle_disconnect();
            printf("h DISCONNECT\n");
        }else if(strcmp(cmd, "GET")==0){
			printf("h GET\n");
        }else{
            send_message(text);
            printf("h MESSAGE\n");
        }

    }

}

void recevie_message(int sig){
	uint type;
	char mes[MAX_MESSAGE_SIZE];
    if(mq_receive(my_queue, mes, MAX_MESSAGE_SIZE, &type)!=-1){

        switch(type){
            case CONNECT:{
            	printf("r CONNECT\n");
                handle_friend_connect(mes);
                break;
            }
            case DISCONNECT:{
            	printf("r DISCONNECT\n");
                handle_friend_disconnect(mes);
                break;
            }
            case MESSAGE:{
            	printf("r MESSAGE\n");
                handle_message(mes);
                break;
            }
            default: break;

        }

    }
    struct sigevent sig_ev;
    sig_ev.sigev_notify = SIGEV_SIGNAL;
    sig_ev.sigev_signo = SIGUSR1;
    mq_notify(my_queue, &sig_ev);

}


void before_exit(){

    mq_close(my_queue);
    mq_unlink(my_name);
    char mes[MAX_MESSAGE_SIZE];
    sprintf(mes, "%d %d", STOP, id);
    if(mq_send(server_queue, mes, strlen(mes), STOP)==-1){
            perror("Can't send message");
    }

    mq_close(server_queue);

    if(friends_queue != -1){
        mq_close(friends_queue);
        char mes[MAX_MESSAGE_SIZE];
        sprintf(mes, "%d %d", DISCONNECT, id);
        if(mq_send(friends_queue, mes, strlen(mes), DISCONNECT)==-1){
            perror("Can't send message");
        }
    }

}

void handle_sigint(int sig){
    exit(0);
}

void handle_stop(){
    raise(SIGINT);
}



void handle_list(){
    char mes[MAX_MESSAGE_SIZE];
    sprintf(mes, "%d %d   ", LIST, id);
    if(mq_send(server_queue, mes, strlen(mes), LIST)==-1){
        perror("Can't send message");
    }

}

void handle_connect(char * text){

    char mes[MAX_MESSAGE_SIZE];
    char second_id[MAX_MESSAGE_SIZE];
    memcpy(second_id, &text[7], strlen(text)-7);
    sprintf(mes, "%d %d %s", CONNECT, id, second_id);

    if(mq_send(server_queue, mes, strlen(mes), CONNECT)==-1){
        perror("Can't send message");
    }

}

void handle_disconnect(){
    char mes[MAX_MESSAGE_SIZE];
    sprintf(mes, "%d %d", DISCONNECT, id);
    if(mq_send(friends_queue, mes, strlen(mes), DISCONNECT)==-1){
        perror("Can't send message");
    }
    if(mq_send(server_queue, mes, strlen(mes), DISCONNECT)==-1){
        perror("Can't send message");
    }
    friends_queue = -1;

}

void send_message(char *text){

    char mes[MAX_MESSAGE_SIZE];
    sprintf(mes, "%d %d %s", MESSAGE, id, text);

    if(mq_send(friends_queue, mes, strlen(mes), MESSAGE)==-1){
        perror("Can't send message");
    }

}

void handle_friend_connect(char *message){
    int type, pid;
    char name[MAX_MESSAGE_SIZE];
    sscanf(message, "%d %d ", &type, &pid);
    sprintf(name, "/q-%d", pid);

    printf("name: %s\n", message);
    printf("pid: %d\n", pid);
    if ((friends_queue = mq_open(name, O_WRONLY)) == -1){
        perror("Can't connect");
        return;
    }

    printf("Connected\n");
}


void handle_friend_disconnect(char *message){
    mq_close(friends_queue);
    friends_queue = -1;
    printf("Disconnected\n");
}

void handle_message(char *message){
    int type;
    char mes[MAX_MESSAGE_SIZE];
    sscanf(message, "%d %s", &type, mes);

    printf("Friend: %s", message);
}