#include "library.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/times.h>

int main(int argc, char **argv){ // arguments: numberOfBlocks, jobList.txt
    //SETTING UP TIMER
    clock_t start_t, end_t;
    static struct tms start_t_cpu;
    static struct tms end_t_cpu;
    long rt, ut, st;
//    printf("%ld", sysconf(_SC_CLK_TCK)); //100

    //CREATING ARRAY
    struct mainArray *array= createArray((int) strtol(argv[1], NULL, 10));

    struct fileSequence sequence;
    struct pair *files = calloc(array->numberOfBlocks, sizeof(struct pair));
    sequence.fileSequence = files;
    sequence.numberOfSequences = array->numberOfBlocks;
    //OPENING LIST OF JOBS
    char *buf = NULL;
    size_t bufSize = 0;
    ssize_t line_size;
    FILE *fp = fopen(argv[2], "r");
    if (!fp)
    {
        fprintf(stderr, "Error opening file '%s'\n", argv[2]);
        return EXIT_FAILURE;
    }



    //CREATING REPORT FILE
    FILE *report = fopen("raport2cw1.txt", "a");
    fputs( "|   Real   |   User   |  System  |\n", report);




    //HANDLING JOBS
    line_size = getline(&buf, &bufSize, fp);

    while (line_size >= 0){

        int p = 0;
        if (strncmp("merge_files", buf, 11) == 0) {
            int beginning = 12;
            for (int i = 12; i < bufSize; ++i) {
                if (buf[i] == ':') {
                    sequence.fileSequence[p].first = calloc(i - beginning + 1, sizeof(char));
                    memcpy(sequence.fileSequence[p].first, &buf[beginning], i - beginning);
                    sequence.fileSequence[p].first[i - beginning + 1] = '\0';
                    beginning = i + 1;
                }
                if (buf[i] == ' ' || buf[i]== '\n') {
                    sequence.fileSequence[p].second = calloc(i - beginning + 1, sizeof(char));
                    memcpy(sequence.fileSequence[p].second, &buf[beginning], i - beginning);
                    sequence.fileSequence[p].second[i - beginning + 1] = '\0';
                    beginning = i + 1;
                    p++;
                }

            }
            mergeAll(array, sequence);

        }
        else if (strncmp("remove_block", buf, 12) == 0) {
            char *idx = calloc(bufSize - 11, sizeof(char));
            memcpy(idx, &buf[13], bufSize - 12);
            int index = (int) strtol(idx, NULL, 10);

            deleteBlock(array, index);
        }
        else if (strncmp("remove_row", buf, 10) == 0) {
            char *block = NULL;
            char *verse = NULL;
            int beginning = 11;
            for (int i = 11; i < bufSize; ++i) {
                if (buf[i] == ' ') {
                    block = calloc(i - beginning + 1, sizeof(char));
                    memcpy(block, &buf[beginning], i - beginning);
                    block[i - beginning + 1] = '\0';
                    beginning = i + 1;
                    break;
                }
            }
            for (int i = beginning; i < bufSize; ++i) {
                if (buf[i] == ' ') {
                    verse = calloc(i - beginning + 1, sizeof(char));
                    memcpy(verse, &buf[beginning], i - beginning);
                    verse[i - beginning + 1] = '\0';

                    beginning = i + 1;
                    break;
                }
            }
            int b = (int) strtol(block, NULL, 10);
            int v = (int) strtol(verse, NULL, 10);

            deleteVerse(array->blocks[b], v);
            }
        else if (strncmp("start_timer", buf, 11) == 0) {
          start_t = times(&start_t_cpu);
        }
        else if (strncmp("stop_timer", buf, 10) == 0){
            end_t = times(&end_t_cpu);
            rt =  end_t - start_t;
            ut =  end_t_cpu.tms_utime - start_t_cpu.tms_utime;
            st =  end_t_cpu.tms_stime - start_t_cpu.tms_stime;
            fprintf(report,"|   %ld    |   %ld    |   %ld    |\n", rt, ut, st );
        }
        else if (strncmp("free_array", buf, 10) == 0) {
            freeMemory(array);
        }

        else if (strncmp("create_array", buf, 12) == 0) {
            char *n = calloc(bufSize - 11, sizeof(char));
            memcpy(n, &buf[13], bufSize - 12);

            int numberOfBlocks = (int) strtol(n, NULL, 10);

            array = createArray(numberOfBlocks);
            files = calloc(array->numberOfBlocks, sizeof(struct pair));
            sequence.fileSequence = files;
            sequence.numberOfSequences = array->numberOfBlocks;
        }
        else if (strncmp("save_block", buf, 10) == 0) {
            char *idx = calloc(bufSize - 9, sizeof(char));
            memcpy(idx, &buf[11], bufSize - 10);
            int index = (int) strtol(idx, NULL, 10);

            fclose(fopen("tmp.txt", "w"));
            saveToFile(array->blocks[index], "tmp.txt");
        }

        else if (strncmp("load_block", buf, 10) == 0) {
            loadFromFile(array, "tmp.txt");
        }

        line_size = getline(&buf, &bufSize, fp);
    }

    free(buf);

    fclose(report);

    freeMemory(array);


}

