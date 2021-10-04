#include <stdio.h>
#include <string.h>
#include <signal.h>
#include <unistd.h>


int main(int argc, char **argv) {
    sigset_t pending;
	if ( strcmp(argv[1], "pending")!=0){
		raise(SIGUSR1);
	}
	if(strcmp(argv[1], "mask")==0 || strcmp(argv[1], "pending")==0) {
		sigpending(&pending);
		printf("Czy sygnał jest widoczny? : %d\n", sigismember( &pending, SIGUSR1));
	}  
}
