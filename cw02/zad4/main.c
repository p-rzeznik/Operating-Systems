#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/times.h>



#define FILENAME_MAX_SIZE  1000
#define MAX_SIZE  100000000


void rewrite(char*,  char*, char*, char*, char);

int main(int argc, char **argv){ //

    if( argc != 5){
        exit(1);
    }

    char *file1 = argv[1];
    char *file2 = argv[2];
    char *s1 = argv[3];
    char *s2 = argv[4];
    FILE *report = fopen("pomiar_zad_4.txt", "w");
    fputs( "|   Variant  |   Real   |   User   |  System  |\n", report);


    clock_t start_t, end_t;
    static struct tms start_t_cpu;
    static struct tms end_t_cpu;
    long rt, ut, st;


    start_t = times(&start_t_cpu);
    rewrite(file1, file2, s1, s2,  'c');
    end_t = times(&end_t_cpu);

    rt =  end_t - start_t;
    ut =  end_t_cpu.tms_utime - start_t_cpu.tms_utime;
    st =  end_t_cpu.tms_stime - start_t_cpu.tms_stime;
    fprintf(report,"|  C library  |   %ld    |   %ld    |   %ld    |\n", rt, ut, st );

    start_t = times(&start_t_cpu);
    rewrite(file1, file2, s1, s2, 's');
    end_t = times(&end_t_cpu);


    rt =  end_t - start_t;
    ut =  end_t_cpu.tms_utime - start_t_cpu.tms_utime;
    st =  end_t_cpu.tms_stime - start_t_cpu.tms_stime;
    fprintf(report,"| sys library |   %ld    |   %ld    |   %ld    |\n", rt, ut, st );

    fclose(report);

    return 0;


}



void rewrite(char *file1, char *file2, char *s1, char *s2,  char mode){
    /*  "c" C library functions (fopen(), fread())
        "s" system functions   (open(), read()) */

    char *buf = calloc(MAX_SIZE, sizeof(char));

    FILE *fp, *fpw;

    int fd,  sz, fdw;
    if(mode == 'c'){
        fp = fopen(file1, "r");
        sz = (int)fread(buf, sizeof(char), MAX_SIZE, fp);

        fpw = fopen(file2,"w");

    }
    else if(mode == 's'){
        fd = open(file1, O_RDONLY);
        sz = read(fd, buf, MAX_SIZE);

        fdw = open(file2, O_CREAT|O_WRONLY|O_TRUNC );
    }
    else{
        printf("invalid mode");
        exit(1);
    }

    ulong len1 = strlen(s1);
    ulong len2 = strlen(s2);



    int beginning = 0;

    for(int i = 0; i<sz-len1; ++i){

        char *tmp = calloc(len1, sizeof(char));
        memcpy(tmp, &buf[i], len1);
        if(strcmp(s1,tmp)==0) {
            char *to_write = calloc(i - beginning , sizeof(char));
            memcpy(to_write, &buf[beginning], i - beginning );
            if(mode=='c'){
                fputs(to_write, fpw);
                fputs(s2,fpw);
            }else {
                write(fdw, to_write, i-beginning);
                write(fdw, s2, len2);
            }
            i += len1;
            beginning = i;
            free(to_write);
        }
        free(tmp);


    }


    if(mode == 'c'){
        fclose(fp);
        fclose(fpw);
    }else {
        close(fd);
        close(fdw);
    }
    free(buf);

}