#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/times.h>


#define FILENAME_MAX_SIZE  1000
#define MAX_SIZE  100000000


void print_files(char*, char*, char);

int main(int argc, char **argv){ //

    char *file1 = calloc(FILENAME_MAX_SIZE, sizeof(char));
    char *file2 = calloc(FILENAME_MAX_SIZE, sizeof(char));

    if( argc == 3){
        file1 = argv[1];
        file2 = argv[2];
    }
    else{
        scanf("%s", file1);
        scanf("%s", file2);
    }

    FILE *report = fopen("pomiar_zad_1.txt", "w");
    fputs( "|   Variant  |   Real   |   User   |  System  |\n", report);


    clock_t start_t, end_t;
    static struct tms start_t_cpu;
    static struct tms end_t_cpu;
    long rt, ut, st;


    start_t = times(&start_t_cpu);
    print_files(file1, file2, 'c');
    end_t = times(&end_t_cpu);

    rt =  end_t - start_t;
    ut =  end_t_cpu.tms_utime - start_t_cpu.tms_utime;
    st =  end_t_cpu.tms_stime - start_t_cpu.tms_stime;
    fprintf(report,"|  C library  |   %ld    |   %ld    |   %ld    |\n", rt, ut, st );

    start_t = times(&start_t_cpu);
    print_files(file1, file2, 's');
    end_t = times(&end_t_cpu);


    rt =  end_t - start_t;
    ut =  end_t_cpu.tms_utime - start_t_cpu.tms_utime;
    st =  end_t_cpu.tms_stime - start_t_cpu.tms_stime;
    fprintf(report,"| sys library |   %ld    |   %ld    |   %ld    |\n", rt, ut, st );

    fclose(report);

    return 0;


}



void print_files(char *file1, char *file2, char mode){
    /*  "c" C library functions (fopen(), fread())
        "s" system functions   (open(), read()) */

    char *buf1 = calloc(MAX_SIZE, sizeof(char));
    char *buf2 = calloc(MAX_SIZE, sizeof(char));
    FILE *fp1;
    FILE *fp2;
    int fd1, fd2, sz1, sz2;
    if(mode == 'c'){
        fp1 = fopen(file1, "r");
        fp2 = fopen(file2, "r");

        sz1 = (int)fread(buf1, sizeof(char), MAX_SIZE, fp1);
        sz2 = (int) fread(buf2, sizeof(char), MAX_SIZE, fp2);
    }
    else if(mode == 's'){
        fd1 = open(file1, O_RDONLY);
        fd2 = open(file2, O_RDONLY);
        sz1 = read(fd1, buf1, MAX_SIZE);
        sz2 = read(fd2, buf2, MAX_SIZE);
    }
    else{
        printf("invalid mode");
        exit(1);
    }


    int v1=1, v2=1;
    for(int i = 0; i < sz1; ++i){
        if(buf1[i] == '\n'){
            v1++;
        }
    }
    for(int i = 0; i < sz2; ++i){
        if(buf2[i] == '\n'){
            v2++;
        }
    }

    int index1 = 0, index2=0;
    int beginning1 = 0, beginning2 = 0;

    if(v1<v2) {
        while (buf1[index1] != '\0') {
            while (buf1[index1] != '\n' && buf1[index1] != '\0') {
                index1 += 1;
            }
            char *verse1 = calloc(index1 - beginning1 + 1, sizeof(char));
            memcpy(verse1, &buf1[beginning1], index1 - beginning1 + 1);
            printf("%s", verse1);
            beginning1 = index1 + 1;
            index1 += 1;
            free(verse1);
            while (buf2[index2] != '\n' && buf2[index2] != '\0') {
                index2 += 1;
            }
            char *verse2 = calloc(index2 - beginning2 + 1, sizeof(char));
            memcpy(verse2, &buf2[beginning2], index2 - beginning2 + 1);
            printf("%s", verse2);
            beginning2 = index2 + 1;
            index2 += 1;
            free(verse2);
        }
        while (buf2[index2] != '\0') {
            while (buf2[index2] != '\n' && buf2[index2] != '\0') {
                index2 += 1;
            }
            char *verse2 = calloc(index2 - beginning2 + 1, sizeof(char));
            memcpy(verse2, &buf2[beginning2], index2 - beginning2 + 1);
            printf("%s", verse2);
            beginning2 = index2 + 1;
            index2 += 1;
            free(verse2);
        }
    } else{
        while (buf2[index2] != '\0') {
            while (buf1[index1] != '\n' && buf1[index1] != '\0') {
                index1 += 1;
            }
            char *verse1 = calloc(index1 - beginning1 + 1, sizeof(char));
            memcpy(verse1, &buf1[beginning1], index1 - beginning1 + 1);
            printf("%s", verse1);
            beginning1 = index1 + 1;
            index1 += 1;
            free(verse1);
            while (buf2[index2] != '\n' && buf2[index2] != '\0') {
                index2 += 1;
            }
            char *verse2 = calloc(index2 - beginning2 + 1, sizeof(char));
            memcpy(verse2, &buf2[beginning2], index2 - beginning2 + 1);
            printf("%s", verse2);
            beginning2 = index2 + 1;
            index2 += 1;
            free(verse2);
        }

        while (buf1[index1] != '\0') {
            while (buf1[index1] != '\n' && buf1[index1] != '\0') {
                index1 += 1;
            }
            char *verse1 = calloc(index1 - beginning1 + 1, sizeof(char));
            memcpy(verse1, &buf1[beginning1], index1 - beginning1 + 1);
            printf("%s", verse1);
            beginning1 = index1 + 1;
            index1 += 1;
            free(verse1);
        }


    }

    if(mode == 'c'){
        fclose(fp1);
        fclose(fp2);
    }else {
        close(fd1);
        close(fd1);
    }
    free(buf1);
    free(buf2);

}
