
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>
#include <sys/time.h>

int width, height;
long pixels;
int **picture;
int **out_picture;
pthread_t *threads;
double *thr_times;
int T;

void *numbers(void *arg);
void *block(void *arg);


int main(int argc, char **argv){ // l. watków, sposob podzialu, nazwa pliku wej., nazwa pliku wyj.


    T= atoi(argv[1]);

    threads = calloc(T, sizeof(pthread_t));
    thr_times = calloc(T, sizeof(double));

    FILE *pic = fopen(argv[3], "r");
    FILE *out = fopen(argv[4], "w");

    char * line = NULL;
    size_t len = 0;

    getline(&line, &len, pic);
    fputs(line, out);

    getline(&line, &len, pic);
    fputs(line, out);

    while(line[0]=='#'){
        getline(&line, &len, pic);
        fputs(line, out);
    }

    sscanf(line, "%d %d", &width, &height);
    pixels = width*height;
    fputs(line, out);

    getline(&line, &len, pic);
    fputs(line, out);


    picture = calloc(height, sizeof(int*));
    out_picture = calloc(height, sizeof(int*));
    for (int h=0; h<height; h++){
        picture[h] = calloc(width, sizeof(int));
        out_picture[h] = calloc(width, sizeof(int));
    }

    int h = 0, w = 0;
    while(getline(&line, &len, pic)){
        char *i;
        char *rest = line;
        while(1){
            i = strtok_r(rest, " \t\r\n", &rest);
            if(i==NULL){break;}
            picture[h][w] = atoi(i);

            w++;
            if (w == width) {
                h++;
                w=0;
                if (h == height) {
                    break;
                }
            }
        }
        if (h == height) {
            break;
        }

    }
    fclose(pic);
    struct timeval begin, end;
    gettimeofday(&begin, 0);

    for(int i=0; i < T; i++){
        if(strcmp(argv[2],"numbers")==0){
            int index = i;
            pthread_create(&threads[i], NULL, numbers, (void *)index);
        }else if(strcmp(argv[2],"block")==0) {
            int index = i;
            pthread_create(&threads[i], NULL, block, (void *)index);
        }else {
            exit(1);
        }
    }



    gettimeofday(&end, 0);

    for (int h=0; h<height; h++){
        for (int w=0; w<width; w++){
            char to_push[4];
            int a = out_picture[h][w];
            if(a<10){
                sprintf(to_push, "   %d", a);
            }else if (a<100){
                sprintf(to_push, "  %d", a);
            }else{
                sprintf(to_push, " %d", a);
            }
            fputs(to_push, out);
        }
    }
    fclose(out);

    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    double total_time = seconds + microseconds*1e-6;

    FILE *times = fopen("times.txt", "a");
    for (int i=0; i<T; i++) {
        fprintf(times, "| %f |", thr_times[i]);
        total_time += thr_times[i];
    }
    fprintf(times, " ---> | total_time: %f | | threads: %d | | %s | | %s |", total_time ,T, argv[3], argv[2]);
    fprintf(times, "\n\n");


    for (int h=0; h<height; h++){
        free(picture[h]);
        free(out_picture[h]);
    }
    free(picture);
    free(out_picture);
    free(threads);
    free(thr_times);

}


void *numbers(void *arg){

    struct timeval begin, end;
    gettimeofday(&begin, 0);
    int index = (int)arg;
    int start_pix = (pixels/T)*index;
    int end_pix = (pixels/T)*(index+1)-1;
    int start_w = start_pix%width;
    int start_h = start_pix/width;
    int end_w = end_pix%width+1;
    int end_h = end_pix/width+1;
    for (int h=start_h; h<end_h; h++){
        for (int w=start_w; w<end_w; w++){
            out_picture[h][w] = 255-picture[h][w];
        }
    }
    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    thr_times[index] = seconds + microseconds*1e-6;

}
void *block(void *arg){

    struct timeval begin, end;
    gettimeofday(&begin, 0);
    int index = (int)arg;
    int start_h = height/T*index;
    int end_h = height/T*(index+1);
    for (int h=start_h; h<end_h; h++){
        for (int w=0; w<width; w++){
            out_picture[h][w] = 255-picture[h][w];
        }
    }
    gettimeofday(&end, 0);
    long seconds = end.tv_sec - begin.tv_sec;
    long microseconds = end.tv_usec - begin.tv_usec;
    thr_times[index] = seconds + microseconds*1e-6;

}
