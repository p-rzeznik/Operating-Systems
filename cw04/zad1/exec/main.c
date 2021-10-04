#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

void handler(){
    printf("Otrzymano sygnał\n");
}

int main(int argc, char **argv) {
    if(argc != 2){
        perror("Niewłaściwa liczba argumentów");
    }
    sigset_t sig, pending;
    if(strcmp(argv[1], "mask")==0 || strcmp(argv[1], "pending")==0){
        sigemptyset(&sig);
        sigaddset(&sig, SIGUSR1);
        if (sigprocmask(SIG_BLOCK, &sig, NULL) < 0)
            perror("Nie udało się zablokować sygnału");

    } else if(strcmp(argv[1], "handler")==0){
        signal(SIGUSR1, handler);

    } else if (strcmp(argv[1], "ignore")==0){
        signal(SIGUSR1, SIG_IGN);
    }

    raise(SIGUSR1);

    if(strcmp(argv[1], "mask")==0 || strcmp(argv[1], "pending")==0) {
        sigpending(&pending);
        printf("Czy sygnał jest widoczny? : %d\n", sigismember( &pending, SIGUSR1));
    }


    execl("./exec", "./exec", argv[1], NULL); 
    


}
