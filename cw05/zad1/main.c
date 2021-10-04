#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_SIZE  100000000
#define MAX_PROG  100
#define MAX_ARGS  50


int main(int argc, char **argv) { // path to file

    if (argc != 2){
        perror("Niewłaściwa liczba argumentów");
    }

    char *buf = NULL;
    size_t bufSize = 0;
    ssize_t line_size;
    pid_t child;
    FILE *fp = fopen(argv[1], "r");

    if (!fp){
        perror("Nie udało się otworzyć pliku");
    }


    line_size = getline(&buf, &bufSize, fp);
    char commands[MAX_PROG][MAX_ARGS];
    while(line_size > 0){

        size_t i = 10;
        while(buf[i] != '='){
            i++;
        }
        i += 2;

        char *line = calloc(line_size - i-1, sizeof(char));
        memcpy(line, &buf[i], line_size - i-1);


        for(int i = 0; i < MAX_PROG; ++i){
            for(int j = 0; j < MAX_ARGS; ++j){
                commands[i][j] = NULL;
            }
        }


        char *prog_ptr = line;
        char *prog = strtok_r(line, "|", &prog_ptr);

        int programs = 0;

        while(prog != NULL){
            char *arg_ptr = prog;
            char *arg = strtok_r(prog, " ", &arg_ptr);
            int args = 0;
            while(arg != NULL){

                commands[programs][args++] = arg;
                arg = strtok_r(NULL, " ", &arg_ptr);
            }

            programs++;
            prog = strtok_r(NULL, "|", &prog_ptr);

            free(arg);
        }

        int pipes[MAX_PROG][2];
        for(int i = 0; i<programs; i++){
            if(pipe(pipes[i])<0){
                perror("Nie udało się utworzyć potoku");
            }
        }


        for(int i = 0; i<programs; i++){

            if((child=fork()) < 0) {
                perror("Nie udało się rozdzielić");

            } else if (child == 0){
                if(i>0){
                    dup2(pipes[i-1][0], STDIN_FILENO);
                }
                if(i < programs-1){
                    dup2(pipes[i][1], STDOUT_FILENO);
                }
                for(int j = 0; j<programs - 1; j++){
                    close(pipes[j][0]);
                    close(pipes[j][1]);
                }

                execvp(commands[i][0], commands[i]);

            }
        }

        for (int i = 0; i < programs - 1; i++){
            close(pipes[i][0]);
            close(pipes[i][1]);
        }

        for (int i = 0; i < programs; i++){
            wait(0);
        }

        line_size = getline(&buf, &bufSize, fp);

    }
    free(buf);
    fclose(fp);
}
