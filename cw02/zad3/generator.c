#include <time.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


int main(int argc, char **argv){

    srand(time(NULL)+ 2310123);
    FILE *dane = fopen("dane.txt", "w");

    const char set[] = "0123456789";
    int size = atoi(argv[1]);
    for(int i = 0; i<size; i++){
        int len = (rand()%5)+1;
        char *line = calloc(len+1, sizeof(char));
        line[0] = set[(rand()%9)+1];
        for(int j = 1; j<len; j++){
            line[j] = set[rand()%10];

        }
        line[len] = '\n';
        fputs(line, dane);
    }



}
