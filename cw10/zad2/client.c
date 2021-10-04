#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define MAX_MESSAGE_LENGTH 256

typedef enum{
    STARTING,
    WAITING,
    WAITING_FOR_MOVE,
    RIVALS_MOVE,
    MOVE,
    QUIT
} State;


pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t cond = PTHREAD_COND_INITIALIZER;

int server_sfd, bind_sfd;
int symbol;
char *name;
char *cmd;
char *arg;

int board[9];
State state = STARTING;


void listen_server(int is_local);
void *play_game();
void check_game();
void quit();
int move(int pos, int sym);
int check_winner();
void reset_board();
void print_board();


int main(int argc, char **argv){ // nazwa, sposob polaczenia, adres serwera

    if (argc != 4){
    	exit(1);
    }

    name = argv[1];
    char *type = argv[2];
    char *address = argv[3];

    signal(SIGINT, quit);
    int is_local = strcmp(type, "local") == 0;

    struct sockaddr_un addr;
    if (is_local){
        server_sfd = socket(AF_UNIX, SOCK_DGRAM, 0);

        memset(&addr, 0, sizeof(addr));
        addr.sun_family = AF_UNIX;
        strcpy(addr.sun_path, address);

        connect(server_sfd, (struct sockaddr *)&addr, sizeof(addr)); 
        bind_sfd = socket(AF_UNIX, SOCK_DGRAM, 0);
        struct sockaddr_un bind_addr;
        memset(&bind_addr, 0, sizeof(bind_addr));
        bind_addr.sun_family = AF_UNIX;
        sprintf(bind_addr.sun_path, "%d", getpid());
        bind(bind_sfd, (const struct sockaddr *) &bind_addr, sizeof(bind_addr));


    }else{
        struct addrinfo *info;

        struct addrinfo hints;
        memset(&hints, 0, sizeof(struct addrinfo));
        hints.ai_family = AF_INET;
        hints.ai_socktype = SOCK_DGRAM;

        getaddrinfo("localhost", address, &hints, &info); 

        server_sfd = socket(info->ai_family, info->ai_socktype, info->ai_protocol);

        connect(server_sfd, info->ai_addr, info->ai_addrlen);

        freeaddrinfo(info);
    }    
    char buf[MAX_MESSAGE_LENGTH + 1];
    sprintf(buf, "add: :%s", name);
    if (is_local)
    {
        sendto(bind_sfd, buf, MAX_MESSAGE_LENGTH, 0, (struct sockaddr *)&addr, sizeof(addr));
    }
    else
    {
        send(server_sfd, buf, MAX_MESSAGE_LENGTH, 0);
    }


    listen_server(is_local);

}

void listen_server(int is_local){
    int running = 0;
    while (1){
        char buf[MAX_MESSAGE_LENGTH + 1];
        if (is_local){
            recv(bind_sfd, buf, MAX_MESSAGE_LENGTH, 0);
        }else{
            recv(server_sfd, buf, MAX_MESSAGE_LENGTH, 0);
        }
        cmd = strtok(buf, ":");
        arg = strtok(NULL, ":");


        pthread_mutex_lock(&mutex);
        if (strcmp(cmd, "add") == 0){
            state = STARTING;
            if (!running)
            {
                pthread_t tid;
                pthread_create(&tid, NULL, play_game, NULL);
                running = 1;
            }
        }else if (strcmp(cmd, "move") == 0){
            state = RIVALS_MOVE;
        }else if (strcmp(cmd, "quit") == 0){
            state = QUIT;
            exit(0);
        }else if (strcmp(cmd, "ping") == 0){
            sprintf(buf, "ping: :%s", name);
            send(server_sfd, buf, MAX_MESSAGE_LENGTH, 0);
        }
        pthread_cond_signal(&cond);
        pthread_mutex_unlock(&mutex);
    }


}
void reset_board(){
	for(int i = 0; i<9; i++){
		board[i]=-1;
	}

}
void print_board(){
    for (int j = 0; j < 3; j++){
        for (int i = 0; i < 3; i++){
            if (board[j * 3 + i] == -1){
                printf("  %d  ", j * 3 + i + 1 );
            }else if (board[j * 3 + i] == 0){
                printf("  O  ");
            }else{
                printf("  X  ");
            }
        }
        printf("\n\n");
    }
    printf("\n________________\n");
}


void *play_game()
{
    while (1)
    {
        if (state == STARTING){
            if (strcmp(arg, "name_taken") == 0){
                exit(1);
            }else if (strcmp(arg, "no_enemy") == 0){
            	printf("Waiting for opponent\n");
                state = WAITING;
            }else{
                reset_board();
                symbol = arg[0] == 'O' ? 0 : 1;
                state = symbol ? MOVE : WAITING_FOR_MOVE;
            }
        }else if (state == WAITING){
            pthread_mutex_lock(&mutex);
            while (state != STARTING && state != QUIT){
                pthread_cond_wait(&cond, &mutex);
            }
            pthread_mutex_unlock(&mutex);

            reset_board();
            symbol = arg[0] == 'O' ? 0 : 1;
            state = symbol ? MOVE : WAITING_FOR_MOVE;
        }else if (state == WAITING_FOR_MOVE){
            printf("Waiting for opponents move\n");

            pthread_mutex_lock(&mutex);
            while (state != RIVALS_MOVE && state != QUIT)
            {
                pthread_cond_wait(&cond, &mutex);
            }
            pthread_mutex_unlock(&mutex);
        }else if (state == RIVALS_MOVE){
            int pos = atoi(arg);
            move(pos, !symbol);
            check_game();
            if (state != QUIT){
                state = MOVE;
            }
        }else if (state == MOVE){
            print_board();
            int pos;
            do{
                printf("Your move: ");
                scanf("%d", &pos);
                pos--;
            } while (!move(pos, symbol));

            print_board();

            char buf[MAX_MESSAGE_LENGTH + 1];
            sprintf(buf, "move:%d:%s", pos, name);
            send(server_sfd, buf, MAX_MESSAGE_LENGTH, 0);

            check_game();
            if (state != QUIT){
                state = WAITING_FOR_MOVE;
            }

        }else if (state == QUIT){
            quit();
        }
    }
}

void quit(){
    char buf[MAX_MESSAGE_LENGTH + 1];
    sprintf(buf, "quit: :%s", name);
    send(server_sfd, buf, MAX_MESSAGE_LENGTH, 0);
    exit(0);
}


void check_game(){
    int end = 0;
    int winner = check_winner(&board);
    if (winner != -1){
        if (symbol == winner){
            printf("You WIN!\n");
        }else{
            printf("You LOST!\n");
        }
        end = 1;
    }

    int draw = 1;
    for (int i = 0; i < 9; i++){
        if (board[i] == -1){
            draw = 0;
            break;
        }
    }
    if (draw && !end){
        printf("DRAW\n");
    }
    if (end || draw){
        state = QUIT;
    }
}

int check_winner(){
    int col = -1;
    for (int i = 0; i < 3; i++){
        if (board[i] == board[i+3] && board[i] == board[i+6] && board[i] != -1)
            col = board[i];
    }
    if (col != -1){
        return col;
	}

    int row = -1;
    for (int j = 0; j < 3; j++){
        if (board[3*j] == board[3*j+1] && board[3*j] == board[3*j+2] && board[3*j] != -1){
            row = board[3*j];
        }
    }
    if (row != -1){
        return row;
    }

    int diag1 = -1;
    if (board[0] == board[4] && board[0] == board[8] && board[0] != -1){
        diag1 = board[0];
    }
    if (diag1 != -1){
        return diag1;
    }

    int diag2 = -1;
    if (board[2] == board[4] && board[2] == board[6] && board[2] != -1){
        diag2 = board[2];
    }
    return diag2;
}

int move(int pos, int sym)
{
    if (pos< 0 || pos > 9 || board[pos] != -1)
        return 0;
    board[pos] = sym;
    return 1;
}