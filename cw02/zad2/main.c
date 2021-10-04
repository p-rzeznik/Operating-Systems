#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/times.h>


#define FILENAME_MAX_SIZE  1000
#define MAX_SIZE  100000000


void print_correct_verses(char*, char, char);

int main(int argc, char **argv){ //

    if( argc != 3){
        exit(1);
    }

    char * file = argv[2];
    char letter = argv[1][0];


    FILE *report = fopen("pomiar_zad_2.txt", "w");
    fputs( "|   Variant  |   Real   |   User   |  System  |\n", report);


    clock_t start_t, end_t;
    static struct tms start_t_cpu;
    static struct tms end_t_cpu;
    long rt, ut, st;


    start_t = times(&start_t_cpu);
    print_correct_verses(file, letter, 'c');
    end_t = times(&end_t_cpu);

    rt =  end_t - start_t;
    ut =  end_t_cpu.tms_utime - start_t_cpu.tms_utime;
    st =  end_t_cpu.tms_stime - start_t_cpu.tms_stime;
    fprintf(report,"|  C library  |   %ld    |   %ld    |   %ld    |\n", rt, ut, st );

    start_t = times(&start_t_cpu);
    print_correct_verses(file, letter, 's');
    end_t = times(&end_t_cpu);


    rt =  end_t - start_t;
    ut =  end_t_cpu.tms_utime - start_t_cpu.tms_utime;
    st =  end_t_cpu.tms_stime - start_t_cpu.tms_stime;
    fprintf(report,"| sys library |   %ld    |   %ld    |   %ld    |\n", rt, ut, st );

    fclose(report);

    return 0;


}



void print_correct_verses(char *file, char letter, char mode){
    /*  "c" C library functions (fopen(), fread())
        "s" system functions   (open(), read()) */

    char *buf = calloc(MAX_SIZE, sizeof(char));

    FILE *fp;

    int fd,  sz;
    if(mode == 'c'){
        fp = fopen(file, "r");
        sz = (int)fread(buf, sizeof(char), MAX_SIZE, fp);

    }
    else if(mode == 's'){
        fd = open(file, O_RDONLY);
        sz = read(fd, buf, MAX_SIZE);

    }
    else{
        printf("invalid mode");
        exit(1);
    }
    int f = 0;
    int beginning = 0;
    for(int i = 0; i<sz; ++i){
        if(buf[i]==letter){
            f = 1;
        }
        if(buf[i]=='\n' || buf[i]=='\0'){
            if(f==1){
                char *verse = calloc(i - beginning + 1, sizeof(char));
                memcpy(verse, &buf[beginning], i - beginning + 1);
                printf("%s", verse);
                free(verse);
            }
            beginning = i + 1;
            f = 0;
        }
    }


    if(mode == 'c'){
        fclose(fp);
    }else {
        close(fd);
    }
    free(buf);

}
