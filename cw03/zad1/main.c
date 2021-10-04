#include <stdio.h>
#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <sys/wait.h>


int main(int argc, char **argv) { // n
    if (argc!=2){
        exit(-1);
    }
    long n = strtol(argv[1], NULL, 10);
    pid_t child_pid;
    int i = 0;

    child_pid = fork();
    while(1) {
        wait(NULL);
        if (child_pid == 0) {
            child_pid = fork();
            i++;
            if (i == n) {
                return 0;
            }
        } else {
            printf("%d. Komunikat z procesu o numerze PID:%d\n", i+1, (int) getpid());
            return 0;
        }
    }

    return 0;

}
