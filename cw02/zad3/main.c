#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>
#include <sys/times.h>
#include <math.h>


#define FILENAME_MAX_SIZE  1000
#define MAX_SIZE  100000000


void write_to_files(char*,  char);

int main(int argc, char **argv){ //

    if( argc != 2){
        exit(1);
    }

    char * file = argv[1];

    FILE *report = fopen("pomiar_zad_3.txt", "w");
    fputs( "|   Variant  |   Real   |   User   |  System  |\n", report);


    clock_t start_t, end_t;
    static struct tms start_t_cpu;
    static struct tms end_t_cpu;
    long rt, ut, st;


    start_t = times(&start_t_cpu);
    write_to_files(file,  'c');
    end_t = times(&end_t_cpu);

    rt =  end_t - start_t;
    ut =  end_t_cpu.tms_utime - start_t_cpu.tms_utime;
    st =  end_t_cpu.tms_stime - start_t_cpu.tms_stime;
    fprintf(report,"|  C library  |   %ld    |   %ld    |   %ld    |\n", rt, ut, st );

    start_t = times(&start_t_cpu);
    write_to_files(file,  's');
    end_t = times(&end_t_cpu);


    rt =  end_t - start_t;
    ut =  end_t_cpu.tms_utime - start_t_cpu.tms_utime;
    st =  end_t_cpu.tms_stime - start_t_cpu.tms_stime;
    fprintf(report,"| sys library |   %ld    |   %ld    |   %ld    |\n", rt, ut, st );

    fclose(report);

    return 0;


}



void write_to_files(char *file,  char mode){
    /*  "c" C library functions (fopen(), fread())
        "s" system functions   (open(), read()) */

    char *buf = calloc(MAX_SIZE, sizeof(char));

    FILE *fp, *fpa, *fpb, *fpc;

    int fd,  sz, fda, fdb, fdc;
    if(mode == 'c'){
        fp = fopen(file, "r");
        sz = (int)fread(buf, sizeof(char), MAX_SIZE, fp);

        fpa = fopen("ac.txt","w");
        fpb = fopen("bc.txt","w");
        fpc = fopen("cc.txt","w");
    }
    else if(mode == 's'){
        fd = open(file, O_RDONLY);
        sz = read(fd, buf, MAX_SIZE);

        fda = open("as.txt", O_CREAT|O_WRONLY|O_TRUNC );
        fdb = open("bs.txt", O_CREAT|O_WRONLY|O_TRUNC );
        fdc = open("cs.txt", O_CREAT|O_WRONLY|O_TRUNC );
    }
    else{
        printf("invalid mode");
        exit(1);
    }

    int v1=1;
    for(int i = 0; i < sz; ++i){
        if(buf[i] == '\n'){
            v1++;
        }
    }


    int even = 0;
    int beginning = 0;

    for(int i = 0; i<sz; ++i){
        if(buf[i]=='\n' || buf[i]=='\0'){
            char *verse = calloc(i - beginning + 1, sizeof(char));
            memcpy(verse, &buf[beginning], i - beginning + 1);
            int num = (int)strtol(verse, NULL, 10);
            if(num%2==0){
                even++;
            }
            if(verse[i-beginning-2]=='0' || verse[i-beginning-2]=='7'){
                if(mode=='c'){
                    fputs(verse, fpb);
                }else {
                    write(fdb, verse, i-beginning+1);
                }
            }

            float sr = sqrt((double)num);
            int sr2 = (int)sr;
            if(num==sr2*sr2){
                if(mode=='c'){
                    fputs(verse, fpc);
                }else {
                    write(fdc, verse, i-beginning+1);
                }
            }

            free(verse);

            beginning = i + 1;
        }

    }

	int len =0;
	int even_cp = even;
	while(even>0){
    	len++;
		even /=10;
    }
    char *str = calloc(len, sizeof(char));
    sprintf(str, "%d", even_cp);

    if(mode=='c'){
        fputs("Liczb parzystych jest ", fpa);
        fputs(str, fpa);
    }else {
        write(fda, "Liczb parzystych jest ", 22);
        write(fda, str, len);
    }

	free(str);

    if(mode == 'c'){
		fclose(fp);
		fclose(fpa);
		fclose(fpb);
		fclose(fpc);
    }else {
        close(fd);
		close(fda);
		close(fdb);
		close(fdc);
    }
    free(buf);

}
