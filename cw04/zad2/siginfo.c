#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>

void handler(int signal, siginfo_t *info, void *ucontext){
    printf("Sygnał: %d\n", signal);
    printf("PID procesu wysyłającego sygnał: %d\n", info->si_pid);
    printf("Czas użytkownika: %ld\n", info->si_utime);
    printf("Czas systemu: %ld\n", info->si_stime);
    printf("Wartość na wyjściu: %d\n", info->si_status);

}

int main(int argc, char **argv) {

    if(argc != 2){
        perror("Niewłaściwa liczba argumentów");
    }

    struct sigaction act;
    sigemptyset(&act.sa_mask);
    act.sa_sigaction = handler;
    act.sa_flags = SA_SIGINFO;
    if(strcmp(argv[1], "sigint")==0 ){
        sigaction(SIGINT, &act, NULL);
        raise(SIGINT);
    } else if(strcmp(argv[1], "sigabrt")==0 ){
        sigaction(SIGABRT, &act, NULL);
        raise(SIGABRT);
    } else if(strcmp(argv[1], "sigusr")==0 ){
        sigaction(SIGUSR1, &act, NULL);
        raise(SIGUSR1);
    }


    return 0;
}
