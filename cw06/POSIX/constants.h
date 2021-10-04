#ifndef CONSTANTS_H
#define CONSTANTS_H

#define MAX_MESSAGE_SIZE 256
#define MAX_CUSTOMERS 128

#define STOP 1
#define DISCONNECT 2
#define LIST 3
#define CONNECT 4
#define INIT 5
#define MESSAGE 6

#define SERVER_STOP 7

#define SERVER_NAME "/server"


struct msg {
	long mtype;
	int id;
	char mtext[MAX_MESSAGE_SIZE];
};


#endif