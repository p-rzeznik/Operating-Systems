#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>



int main(int argc, char **argv){ // przyjmuje cztery argumenty: ścieżka do potoku nazwanego, numer wiersza, ścieżka do pliku tekstowego z dowolną zawartością, N - liczba znaków odczytywanych jednorazowo z pliku

    if(argc != 5){
        perror("Niewłaściwa liczba argumentówp");
    }
    char *pipe_path = argv[1];
    int verse = strtol(argv[2], NULL, 10);
    char *file_path = argv[3];
    int N = strtol(argv[4], NULL, 10);

    FILE *file = fopen(file_path, "r");

    int pipe_fd = open(pipe_path, O_WRONLY);

    char buf[N+1];

    while(fgets(buf, N+1, file)!=NULL){

        char m[N+10];
        sprintf(m, "%d %s\n", verse, buf);
        write(pipe_fd, m, strlen(m));
        sleep(2);

    }

    close(pipe_fd);
    fclose(file);
    return 0;
}