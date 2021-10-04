#include <netdb.h>
#include <poll.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <time.h>
#include <unistd.h>

#define MAX_PLAYERS 20
#define MAX_MESSAGE_LENGTH 256

pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;

typedef struct
{
    char *name;
    int fd;
    int online;
    struct sockaddr addr;
} client;


int network_sfd, local_sfd;
client *clients[MAX_PLAYERS] = {NULL};
int num_clients = 0;

void *ping();
void remove_offline_clients();
void ping_to_clients();
int get_opponent(int index);
int receive_message();
int get_client(char *name);
int add_client(char *name, int fd, struct sockaddr addr);
void free_client(int index);

int main(int argc, char **argv){ // nr portu, sciezka

    srand(time(NULL));

	if (argc!=3){
		exit(1);
	}
    char *port = argv[1];
    char *path = argv[2];

    //initialize network socket
    struct addrinfo hints;
    struct addrinfo *result;
	memset(&hints, 0, sizeof(hints));
    hints.ai_family = AF_INET;   
    hints.ai_socktype = SOCK_DGRAM; 
    hints.ai_flags = AI_PASSIVE;    

    getaddrinfo(NULL, port, &hints, &result);
    network_sfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    bind(network_sfd, result->ai_addr, result->ai_addrlen);
    listen(network_sfd, 10);
    freeaddrinfo(result);

    //initialize local socket
    local_sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
    struct sockaddr_un name;
    memset(&name, 0, sizeof(name));
    name.sun_family = AF_UNIX;		
    strcpy(name.sun_path, path);
    unlink(path);
    bind(local_sfd, (const struct sockaddr *) &name, sizeof(name));
    listen(local_sfd, 10);


    // create ping thread
    pthread_t thread;
    pthread_create(&thread, NULL, ping, NULL);

	while (1){
        int fd = receive_message(local_sfd, network_sfd);
        char buf[MAX_MESSAGE_LENGTH + 1];
        struct sockaddr addr;
        socklen_t len = sizeof(struct sockaddr);
        recvfrom(fd, buf, MAX_MESSAGE_LENGTH, 0, &addr, &len);

        char *cmd = strtok(buf, ":");
        char *arg = strtok(NULL, ":");
        char *name = strtok(NULL, ":"); 

        pthread_mutex_lock(&mutex);
        if (strcmp(cmd, "add") == 0){
            int index = add_client(name, fd, addr);

            if (index == -1){
                sendto(fd, "add:name_taken", MAX_MESSAGE_LENGTH, 0, (struct sockaddr *)&addr, sizeof(addr));
            }else if (index % 2 == 0){
                sendto(fd, "add:no_enemy", MAX_MESSAGE_LENGTH, 0, (struct sockaddr *)&addr, sizeof(addr));
            }else{   

                int first, second;
                if (rand() % 2 == 0){
                    first = index;
                    second = get_opponent(index);
                }else{
                    second = index;
                    first = get_opponent(index);
                }
                sendto(clients[first]->fd, "add:O", MAX_MESSAGE_LENGTH, 0,
                 &clients[first]->addr, sizeof(struct addrinfo));
                sendto(clients[second]->fd, "add:X",MAX_MESSAGE_LENGTH, 0,
                 &clients[second]->addr, sizeof(struct addrinfo));
            
            }
        }else if (strcmp(cmd, "move") == 0){
            int move = atoi(arg);
            int cl = get_client(name);

            sprintf(buf, "move:%d", move);
            sendto(clients[get_opponent(cl)]->fd, buf, MAX_MESSAGE_LENGTH, 0,
                &clients[get_opponent(cl)]->addr, sizeof(struct addrinfo));

        }else if (strcmp(cmd, "quit") == 0){
            int cl = get_client(name);
            if (cl != -1){
               free_client(cl);
           }
        }else if (strcmp(cmd, "ping") == 0){
            int cl = get_client(name);
            if (cl != -1){
                clients[cl]->online = 1;
            }
        }
        pthread_mutex_unlock(&mutex);

    }
    return 0;

}
int add_client(char *name, int fd, struct sockaddr addr){

    for (int i = 0; i < MAX_PLAYERS; i++){
        if (clients[i] != NULL && strcmp(clients[i]->name, name) == 0){
            return -1;
        }
    }

    int index = -1;
    for (int i = 0; i < MAX_PLAYERS; i += 2){
        if (clients[i] != NULL && clients[i + 1] == NULL){
            index = i + 1;
            break;
        }
    }

    if (index == -1){
        for (int i = 0; i < MAX_PLAYERS; i++){
            if (clients[i] == NULL){
                index = i;
                break;
            }
        }
    }

    if (index != -1){
        client *new_client = calloc(1, sizeof(client));
        new_client->name = calloc(MAX_MESSAGE_LENGTH, sizeof(char));
        strcpy(new_client->name, name);
        new_client->fd = fd;
        new_client->online = 1;
        new_client->addr = addr;
        clients[index] = new_client;
        num_clients++;
    }

    return index;



}
int get_client(char *name){
    for (int i = 0; i < MAX_PLAYERS; i++){
        if (clients[i] != NULL && strcmp(clients[i]->name, name) == 0){
            return i;
        }
    }
    return -1;
}

int receive_message(){
    struct pollfd *fds = calloc(2 , sizeof(struct pollfd));

    fds[0].fd = local_sfd;
    fds[0].events = POLLIN;
    fds[1].fd = network_sfd;
    fds[1].events = POLLIN;

    poll(fds, 2, -1);

    for (int i = 0; i < 2; i++){
        if (fds[i].revents & POLLIN){
            return fds[i].fd;
            
        }
    }


    return -1;
}

void *ping(){
	while(1){
		pthread_mutex_lock(&mutex);
		remove_offline_clients();
		ping_to_clients();
        pthread_mutex_unlock(&mutex);
        sleep(5);
	}
}
void free_client(int index)
{
    free(clients[index]->name);
    free(clients[index]);
    clients[index] = NULL;
    num_clients--;
    int opponent = get_opponent(index);
    if (clients[opponent] != NULL)
    {
        sendto(clients[opponent]->fd, "quit: ", MAX_MESSAGE_LENGTH, 0,
               &clients[opponent]->addr, sizeof(struct addrinfo));
        free(clients[opponent]->name);
        free(clients[opponent]);
        clients[opponent] = NULL;
        num_clients--;
    }
}
void remove_offline_clients(){

    for (int i = 0; i < MAX_PLAYERS; i++){
        if (clients[i] != NULL && !clients[i]->online){
            free(clients[i]);
        }
    }
}
void ping_to_clients(){
    for (int i = 0; i < MAX_PLAYERS; i++){
        if (clients[i] != NULL){
            sendto(clients[i]->fd, "ping: ", MAX_MESSAGE_LENGTH, 0, &clients[i]->addr, sizeof(struct addrinfo));
            clients[i]->online = 0;
        }
    }
}

int get_opponent(int index){
    if (index % 2 == 0)
        return index + 1;
    else
        return index - 1;
}
