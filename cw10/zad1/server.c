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
int add_client(char *name, int fd);
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
    hints.ai_family = AF_UNSPEC;   
    hints.ai_socktype = SOCK_STREAM; 
    hints.ai_flags = AI_PASSIVE;   

    getaddrinfo(NULL, port, &hints, &result);
    network_sfd = socket(result->ai_family, result->ai_socktype, result->ai_protocol);
    bind(network_sfd, result->ai_addr, result->ai_addrlen);
    listen(network_sfd, 10);
    freeaddrinfo(result);

    //initialize local socket
    local_sfd = socket(AF_UNIX, SOCK_STREAM, 0);
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
        int client_fd = receive_message(local_sfd, network_sfd);
        char buf[MAX_MESSAGE_LENGTH + 1];
        recv(client_fd, buf, MAX_MESSAGE_LENGTH, 0);

        char *cmd = strtok(buf, ":");
        char *arg = strtok(NULL, ":");
        char *name = strtok(NULL, ":"); 

        pthread_mutex_lock(&mutex);
        if (strcmp(cmd, "add") == 0){
            int index = add_client(name, client_fd);
            if (index == -1){
                send(client_fd, "add:name_taken", MAX_MESSAGE_LENGTH, 0);
                close(client_fd);
            }else if (index % 2 == 0){
                send(client_fd, "add:no_enemy", MAX_MESSAGE_LENGTH, 0);
            }else{   

                int first, second;
                if (rand() % 2 == 0){
                    first = index;
                    second = get_opponent(index);
                }else{
                    second = index;
                    first = get_opponent(index);
                }
                send(clients[first]->fd, "add:O", MAX_MESSAGE_LENGTH, 0);
                send(clients[second]->fd, "add:X", MAX_MESSAGE_LENGTH, 0);
            }
        }else if (strcmp(cmd, "move") == 0){
            int move = atoi(arg);
            int player = get_client(name);

            sprintf(buf, "move:%d", move);
            send(clients[get_opponent(player)]->fd, buf, MAX_MESSAGE_LENGTH,
                 0);
        }else if (strcmp(cmd, "quit") == 0){
            int player = get_client(name);
            if (player != -1){
               free_client(player);
           }
        }else if (strcmp(cmd, "ping") == 0){
            int player = get_client(name);
            if (player != -1){
                clients[player]->online = 1;
            }
        }
        pthread_mutex_unlock(&mutex);

    }
    return 0;

}
int add_client(char *name, int fd){

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
    struct pollfd *fds = calloc(2 + num_clients, sizeof(struct pollfd));

    pthread_mutex_lock(&mutex);
    fds[0].fd = local_sfd;
    fds[0].events = POLLIN;
    fds[1].fd = network_sfd;
    fds[1].events = POLLIN;
    for (int i = 0; i < num_clients; i++){
        fds[i + 2].fd = clients[i]->fd;
        fds[i + 2].events = POLLIN;
    }
    pthread_mutex_unlock(&mutex);

    poll(fds, num_clients + 2, -1);

    int fd;
    for (int i = 0; i < num_clients + 2; i++){
        if (fds[i].revents & POLLIN){
            fd = fds[i].fd;
            break;
        }
    }

    if (fd == network_sfd || fd == local_sfd){
        fd = accept(fd, NULL, NULL);
    }
    free(fds);

    return fd;
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
	
    	send(clients[opponent]->fd, "quit: ", MAX_MESSAGE_LENGTH, 0);
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
            send(clients[i]->fd, "ping: ", MAX_MESSAGE_LENGTH, 0);
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
