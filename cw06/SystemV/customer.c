#include <stdio.h>
#include <sys/msg.h> 
#include <sys/ipc.h>
#include <unistd.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "constants.h"

int server_queue;
int my_queue;
int friends_queue = -1;
int id;


void before_exit();
void handle_sigint(int sig);
void handle_stop();
int is_queue_empty();
void handle_list();
void handle_connect(char * text);
void handle_disconnect();
void send_message(char *text);
void handle_friend_connect(struct msg *message);
void handle_friend_disconnect(struct msg *message);
void handle_message(struct msg *message);


int main(int argc, char **argv){

	atexit(before_exit);

    key_t server_key;

    if ((server_key = ftok(getenv("HOME"), SERVER_ID)) == -1){
        perror("Can't generate key");
    }
    if ((server_queue = msgget(server_key, 0)) == -1){
        perror("Can't open server queue");
        exit(1);
    }

    key_t my_key;

    if ((my_key = ftok(getenv("HOME"), getpid())) == -1){
        perror("Can't generate key");
    }
    if ((my_queue = msgget(my_key, IPC_CREAT | IPC_EXCL | 0600)) == -1){
        perror("Can't create queue");
        exit(1);
    }

	struct msg mes;
	mes.mtype = INIT;
	sprintf(mes.mtext, "%d", my_key);


	if(msgsnd(server_queue, &mes, sizeof(struct msg) - sizeof(long), 0)==-1){
		perror("Can't send message");
		exit(1);
	}

	struct msg mes_received;

	if(msgrcv(my_queue, &mes_received, sizeof(struct msg) - sizeof(long), -10, 0)==-1){
		perror("Can't receive message");
		exit(1);
	}

	id = mes_received.id;

	printf("Your ID: %d\n",id);

	struct sigaction sa1;
    sigemptyset(&sa1.sa_mask);
    sa1.sa_flags = 0;
    sa1.sa_handler = handle_sigint;
    sigaction(SIGINT, &sa1, NULL);


    while(1){

    	char text[1024];
	    char text_copy[1024];

    	fgets(text, 1024, stdin);

	    strcpy(text_copy, text);

	    char *text_tmp = text_copy;

    	char *cmd = strtok_r(text_copy, " \n", &text_tmp);

    	if(strcmp(cmd, "STOP")==0){
    		handle_stop();
    	}else if (strcmp(cmd, "LIST")==0){
    		handle_list();
    	}else if(strcmp(cmd, "CONNECT")==0){
    		handle_connect(text);
    	}else if(strcmp(cmd, "DISCONNECT")==0){
    		handle_disconnect();
    	}else if(strcmp(cmd, "GET")==0){
			
	   	}else{
    		send_message(text);
    	}


    	while(!is_queue_empty()){

    		struct msg mes;

    		if (msgrcv(my_queue, &mes, sizeof(struct msg) - sizeof(long), -10, IPC_NOWAIT)!=-1){

    			switch(mes.mtype){
    				case CONNECT:{
    					handle_friend_connect(&mes);
    					break;
    				}
    				case DISCONNECT:{
    					handle_friend_disconnect(&mes);
    					break;
					}
					case MESSAGE:{
						handle_message(&mes);
						break;
					}
					default: break;


    			}

    		}

    	}


    }

}


void before_exit(){

	struct msg mes;
	mes.mtype = STOP;
	mes.id = id;

	if(msgsnd(server_queue, &mes, sizeof(struct msg) - sizeof(long), 0)==-1){
		perror("Can't send message");
	}


	if (msgctl(my_queue, IPC_RMID, NULL)==-1){
		perror("Can't remove queue");
	}

	if(friends_queue != -1){
		mes.mtype = DISCONNECT;
		if(msgsnd(friends_queue, &mes, sizeof(struct msg) - sizeof(long), 0)==-1){
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

int is_queue_empty(){
	struct msqid_ds buf;
    msgctl(my_queue, IPC_STAT, &buf);
    return buf.msg_qnum == 0;
}

void handle_list(){
	struct msg mes;
	mes.mtype = LIST;
	mes.id = id;
	if(msgsnd(server_queue, &mes, sizeof(struct msg) - sizeof(long), 0)==-1){
		perror("Can't send message");
	}

}

void handle_connect(char * text){

	struct msg mes;
	mes.mtype = CONNECT;
	mes.id = id;
	memcpy(mes.mtext, &text[7], strlen(text)-8);

	if(msgsnd(server_queue, &mes, sizeof(struct msg) - sizeof(long), 0)==-1){
		perror("Can't send message");
	}

}

void handle_disconnect(){
	struct msg mes;
	mes.mtype = DISCONNECT;
	mes.id = id;
	if(msgsnd(friends_queue, &mes, sizeof(struct msg) - sizeof(long), 0)==-1){
		perror("Can't send message");
	}
	if(msgsnd(server_queue, &mes, sizeof(struct msg) - sizeof(long), 0)==-1){
		perror("Can't send message");
	}
	friends_queue = -1;


}

void send_message(char *text){

	struct msg mes;
	mes.mtype = MESSAGE;
	mes.id = id;
	sprintf(mes.mtext, "%s", text);
	if(msgsnd(friends_queue, &mes, sizeof(struct msg) - sizeof(long), 0)==-1){
		perror("Can't send message");
	}

}

void handle_friend_connect(struct msg *message){

	friends_queue = strtol(message->mtext, NULL, 10);
	if(friends_queue==-1){
		perror("Can't connect");
		return;
	}
	printf("Connected\n");
}

void handle_friend_disconnect(struct msg *message){
	friends_queue = -1;
	printf("Disconnected\n");
}

void handle_message(struct msg *message){
	printf("Friend: %s", message->mtext);
}
