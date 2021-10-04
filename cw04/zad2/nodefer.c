#include <stdio.h>
#include <string.h>
#include <signal.h>

void handler(int signal){
    sigset_t mask, old;
    sigemptyset(&mask);
    sigprocmask(SIG_SETMASK, &mask, &old);
    printf("Czy sygnał jest widoczny? : %d\n", sigismember( &old, SIGINT));

}
int main() {

    struct sigaction act1;
    sigemptyset(&act1.sa_mask);
    act1.sa_handler = handler;
    act1.sa_flags = SA_NODEFER;

    sigaction(SIGINT, &act1, NULL);
    raise(SIGINT);

    struct sigaction act2;
    sigemptyset(&act2.sa_mask);
    act2.sa_handler = handler;
    act2.sa_flags = 0;


    sigaction(SIGINT, &act2, NULL);
    raise(SIGINT);

    return 0;
}