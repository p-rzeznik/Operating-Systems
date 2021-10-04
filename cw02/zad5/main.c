#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/times.h>
#include <math.h>


#define FILENAME_MAX_SIZE  1000
#define MAX_SIZE  100000000


void rewrite(char*,  char*, char);

int main(int argc, char **argv){ //

    if( argc != 3){
        exit(1);
    }

    char *file1 = argv[1];
    char *file2 = argv[2];
    FILE *report = fopen("pomiar_zad_5.txt", "w");
    fputs( "|   Variant  |   Real   |   User   |  System  |\n", report);


    clock_t start_t, end_t;
    static struct tms start_t_cpu;
    static struct tms end_t_cpu;
    long rt, ut, st;


    start_t = times(&start_t_cpu);
    rewrite(file1, file2,  'c');
    end_t = times(&end_t_cpu);

    rt =  end_t - start_t;
    ut =  end_t_cpu.tms_utime - start_t_cpu.tms_utime;
    st =  end_t_cpu.tms_stime - start_t_cpu.tms_stime;
    fprintf(report,"|  C library  |   %ld    |   %ld    |   %ld    |\n", rt, ut, st );

    start_t = times(&start_t_cpu);
    rewrite(file1, file2, 's');
    end_t = times(&end_t_cpu);


    rt =  end_t - start_t;
    ut =  end_t_cpu.tms_utime - start_t_cpu.tms_utime;
    st =  end_t_cpu.tms_stime - start_t_cpu.tms_stime;
    fprintf(report,"| sys library |   %ld    |   %ld    |   %ld    |\n", rt, ut, st );

    fclose(report);

    return 0;


}



void rewrite(char *file1, char *file2,  char mode){
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




    int counter = 0;
    int beginning = 0;

    for(int i = 0; i<sz; ++i){
        counter++;
        if(buf[i]=='\n' && counter==1){
            i++;
            beginning++;
        }

        if(buf[i]=='\n' || buf[i]=='\0' || counter==50) {
            char *verse = calloc(i - beginning + 2, sizeof(char));
            memcpy(verse, &buf[beginning], i - beginning +1);
            if(counter==50) {
                verse[i - beginning + 1] = '\n';
            }
            if(mode=='c'){
                fputs(verse, fpw);
            }else {
                if(counter==50) {
                    write(fdw, verse, i-beginning+2);
                }else if( buf[i]=='\n'){
                    write(fdw, verse, i-beginning+1);
                }else{
                    write(fdw, verse, i-beginning);
		}
            }

            free(verse);
            counter = 0;
            beginning = i + 1;
        }



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
