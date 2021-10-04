#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>

long counter = 0;
long n;
char *mode;

void handle_usr1(int sig, siginfo_t *info, void *ucontext_t){
    counter++;
}

void handle_usr2(int sig, siginfo_t *info, void *ucontext_t){
    printf("Sender:");
    printf("Oczekiwana liczba sygnałów: %ld\n", n);
    printf("Otrzymana liczba sygnałów: %ld\n", counter);
    exit(0);

}

int main(int argc, char **argv) { // PID,  liczba sygnałow, tryb

    if (argc != 4) {
        perror("Niewłaściwa liczba argumentów");
        exit(1);
    }

    pid_t catcher_pid = strtol(argv[1], NULL, 10);
    n = strtol(argv[2], NULL, 10);
    mode = argv[3];

    if(strcmp(argv[3], "kill")==0){
        for (int i =0; i<n; i++){
            kill(catcher_pid, SIGUSR1);
        }
        kill(catcher_pid, SIGUSR2);
    } else if(strcmp(argv[3], "sigqueue")==0){
        for (int i =0; i<n; i++){
            sigqueue(catcher_pid, SIGUSR1, (union sigval) NULL);
        }
        sigqueue(catcher_pid, SIGUSR2, (union sigval) NULL);

    } else if(strcmp(argv[3], "sigrt")==0){
        for (int i =0; i<n; i++){
            kill(catcher_pid, SIGRTMIN+1);
        }
        kill(catcher_pid, SIGRTMIN+2);
    } else{
        exit(1);
    }

    sigset_t mask;

    struct sigaction act1;
    struct sigaction act2;


    if(strcmp(mode, "kill")==0 || strcmp(mode, "sigqueue")==0) {

		sigfillset(&mask);
		sigdelset(&mask, SIGUSR1);
		sigdelset(&mask, SIGUSR2);
		if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
			perror("Nie udało się zablokować sygnału");
		}
		
		act1.sa_mask = mask;
		act1.sa_sigaction = handle_usr1;
		act1.sa_flags = SA_SIGINFO;


		act2.sa_mask = mask;
		act2.sa_sigaction = handle_usr2;
		act2.sa_flags = SA_SIGINFO;
		
	    sigaction(SIGUSR1, &act1, NULL);
	    sigaction(SIGUSR2, &act2, NULL);
	    
		    
    } else{
    
    
		sigfillset(&mask);
		sigdelset(&mask, SIGRTMIN+1);
		sigdelset(&mask, SIGRTMIN+2);
		if (sigprocmask(SIG_BLOCK, &mask, NULL) < 0) {
		    perror("Nie udało się zablokować sygnału");
		}
		
		act1.sa_mask = mask;
		act1.sa_sigaction = handle_usr1;
		act1.sa_flags = SA_SIGINFO;


		act2.sa_mask = mask;
		act2.sa_sigaction = handle_usr2;
		act2.sa_flags = SA_SIGINFO;    
		
        sigaction(SIGRTMIN+1, &act1, NULL);
        sigaction(SIGRTMIN+2, &act2, NULL);
        
    }

    while(1){
        usleep(100);
    }

    return 0;
}
