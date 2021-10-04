#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>
#include <unistd.h>
#include <sys/wait.h>


int main(int argc, char **argv){ // arguments: numberOfBlocks, sequences of files 
    //SETTING UP TIMER
    clock_t start_t, end_t;
    static struct tms start_t_cpu;
    static struct tms end_t_cpu;
    long rt, ut, st;
//    printf("%ld", sysconf(_SC_CLK_TCK)); //100

    //OPENING REPORT FILE
    FILE *report = fopen("raport2.txt", "a");

    //CREATING ARRAY
    struct mainArray *array= createArray((int) strtol(argv[1], NULL, 10));

    struct fileSequence sequence;
    struct pair *files = calloc(array->numberOfBlocks, sizeof(struct pair));
    sequence.fileSequence = files;
    sequence.numberOfSequences = array->numberOfBlocks;

    start_t = times(&start_t_cpu);

    // CREATING SEQUENCE
    for(int p = 2; p<argc;p++) {

        char *buf = argv[p];

        int beginning = 0;
        int i = 0;
        while (buf[i] != '\0') {
            if (buf[i] == ':') {
                sequence.fileSequence[p-2].first = calloc(i - beginning + 1, sizeof(char));
                memcpy(sequence.fileSequence[p-2].first, &buf[beginning], i - beginning);
//                sequence.fileSequence[p].first[i - beginning + 1] = '\0';
                beginning = i + 1;
            }

            i++;
        }
        sequence.fileSequence[p-2].second = calloc(i - beginning + 1, sizeof(char));
        memcpy(sequence.fileSequence[p-2].second, &buf[beginning], i - beginning);
//        sequence.fileSequence[p].second[i - beginning + 1] = '\0';

    }

    pid_t child_pid;
    for(int i = 0; i<sequence.numberOfSequences; ++i){
        child_pid = fork();
        if (child_pid == 0) {
            array->blocks[i] = mergePair(sequence.fileSequence[i]);
            exit(1);
        } else {
            wait(NULL);
        }

    }

    end_t = times(&end_t_cpu);
    rt =  end_t - start_t;
    ut =  end_t_cpu.tms_utime - start_t_cpu.tms_utime + end_t_cpu.tms_cutime - start_t_cpu.tms_cutime;
    st =  end_t_cpu.tms_stime - start_t_cpu.tms_stime + end_t_cpu.tms_cstime - start_t_cpu.tms_cstime;
    fprintf(report,"|   %ld    |   %ld    |   %ld    |\n", rt, ut, st );


    fclose(report);

    freeMemory(array);


}

/*

 Zmierzone czasy w zadaniu 2 ćwiczenia 3 są bardzo podobne do czasów zmierzonych w ćwiczeniu 1.

*/

