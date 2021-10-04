#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>

#define MAX_PRODUCERS 100

void append_to_i_line(FILE *inf, FILE *outf, const char *suffix, int verse) {
    int ch, v = 1;
    while(1) {
        ch = fgetc(inf);
        if (ch == '\n' || ch == EOF) {
            if(v == verse){
                fputs(suffix, outf);
            }
            if (ch == EOF) {
                break;
            }
            v++;
        }
        fputc(ch, outf);
    }
}

int main(int argc, char **argv){ //przyjmuje trzy argumenty: ścieżka do potoku nazwanego, ścieżka do pliku tekstowego (do którego będzie zapisywany odczytany tekst), N — liczba znaków odczytywanych jednorazowo z pliku

    if(argc != 4){
        perror("Niewłaściwa liczba argumentów");
    }
    char *pipe_path = argv[1];
    char *file_path = argv[2];
    int N = strtol(argv[3], NULL, 10);

    FILE *f = fopen(file_path, "w");

    for(int i = 0; i<MAX_PRODUCERS; i++){
        fputc('\n', f);
    }
    fclose(f);

    FILE *pipe = fopen(pipe_path, "r");

    char *buf = NULL;
    size_t bufSize = 0;
    ssize_t line_size;

    while((line_size = getline(&buf, &bufSize, pipe))>0){
        char *line_ptr = buf;
        char *line = strtok_r(buf, " ", &line_ptr);
        int verse = strtol(line, NULL, 10);
        char *to_write = calloc(N, sizeof(char));
        if (strlen(line_ptr)==1){
          break;
        } else if(strlen(line_ptr)<=N){
            memcpy(to_write, line_ptr, strlen(line_ptr)-1);
        } else{
            memcpy(to_write, line_ptr, N);
        }


        FILE *file = fopen(file_path, "r");
        char *tmp = "tmp.txt";

        FILE *outfile = fopen(tmp, "w");

        append_to_i_line(file, outfile, to_write, verse);

        fclose(file);
        fclose(outfile);
        remove(file_path);
        rename(tmp, file_path);
    }

    fclose(pipe);
    return 0;

}