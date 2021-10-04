#include "library.h"

#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>


#define MAX_SIZE  10000000000

struct mainArray *createArray(int numberOfBlocks){
    if(numberOfBlocks<0){
        return NULL;
    }

    struct mainArray *array = calloc(1, sizeof(struct mainArray));

    array->numberOfBlocks = numberOfBlocks;
    struct block **blocks = calloc(numberOfBlocks, sizeof(struct block*));
    array->blocks = blocks;

    return array;
}


void mergeAll(struct mainArray *array, struct fileSequence files){
    for(int i = 0; i<files.numberOfSequences; ++i){
        array->blocks[i] = mergePair(files.fileSequence[i]);
    }
}


struct block *mergePair(struct pair files){
    //READING FROM FILE
    char *file1 = calloc(MAX_SIZE, sizeof(char));
    char *file2 = calloc(MAX_SIZE, sizeof(char));
    int fd1, sz1,fd2, sz2;
    fd1 = open(files.first, O_RDONLY);
    if (fd1 < 0) { perror("r1"); exit(1); }
    sz1 = read(fd1, file1, MAX_SIZE);
    fd2 = open(files.second, O_RDONLY);
    if (fd2 < 0) { perror("r1"); exit(1); }
    sz2 = read(fd2, file2, MAX_SIZE);
    file1[sz1] = '\0';
    file2[sz2] = '\0';

    //CALCULATING LENGTH OF BLOCK
    int v1=1, v2=1;
    for(int i = 0; i < sz1; ++i){
        if(file1[i] == '\n'){
            v1++;
        }
    }
    for(int i = 0; i < sz2; ++i){
        if(file2[i] == '\n'){
            v2++;

        }
    }

    //CREATING BLOCK
    struct block *block = calloc(1, sizeof(struct block));
    char **verses = calloc(v1+v2, sizeof(char*));
    block->verses = verses;
    block->numberOfVerses = v1+v2;

    //MERGING FILES
    int end1 = 0, end2=0;
    int beginning = 0;
    int minV = v1 < v2 ? v1 : v2;
    int n = 0;
    for(int i = 0; i <= sz1; ++i){
        if(file1[i] == '\n'|| file1[i] == '\0'){
            char *verse = calloc(i-beginning+1, sizeof(char));
            memcpy(verse, &file1[beginning], i-beginning);
            verse[i-beginning+1] = '\0';
            block->verses[2*n] = verse;
            beginning = i+1;
            n++;
            if(n==minV){
                end1 = beginning;
                break;
            }
        }
    }

    beginning = 0;
    n = 0;
    for(int i = 0; i <= sz2; ++i) {
        if (file2[i] == '\n' || file2[i] == '\0') {
            char *verse = calloc(i - beginning + 1, sizeof(char));
            memcpy(verse, &file2[beginning], i - beginning);
            verse[i - beginning + 1] = '\0';
            block->verses[2 * n + 1] = verse;
            beginning = i + 1;
            n++;
            if(n==minV){
                end2 = beginning;
                break;
            }
        }
    }
    if(v1 != v2) {
        minV = 2*minV;
        if (v1 < v2) {
            beginning = end2;
            for (int i = end2; i <= sz2; ++i) {
                if (file2[i] == '\n'|| file2[i] == '\0') {
                    char *verse = calloc(i - beginning + 1, sizeof(char));
                    memcpy(verse, &file2[beginning], i - beginning);
                    verse[i - beginning + 1] = '\0';
                    block->verses[minV] = verse;
                    beginning = i + 1;
                    minV++;
                }
            }
        } else {
            beginning = end1;
            for (int i = end1; i <= sz1; ++i) {
                if (file1[i] == '\n'|| file1[i] == '\0') {
                    char *verse = calloc(i - beginning + 1, sizeof(char));
                    memcpy(verse, &file1[beginning], i - beginning);
                    verse[i - beginning + 1] = '\0';
                    block->verses[minV] = verse;
                    beginning = i + 1;
                    minV++;
                }
            }
        }
    }

    return block;

}
void printMergedFiles(struct mainArray *array){
    for(int i = 0; i< array->numberOfBlocks; ++i){
        printf("Pair %d\n", i+1);
        struct block *blocks = array->blocks[i];
        for(int j = 0; j<blocks->numberOfVerses; ++j){
            printf("%s\n", blocks->verses[j]);
        }
    }
}

void saveToFile(struct block *block,  char * path) {
    FILE *fp = fopen(path, "a");

    for (int j = 0; j < block->numberOfVerses; ++j) {
        fputs(block->verses[j], fp);
        fputc('\n', fp);
    }
    fclose(fp);
}

int loadFromFile(struct mainArray *array,  char * path) {
    char *file1 = calloc(MAX_SIZE, sizeof(char));
    int fd1, sz1;
    fd1 = open(path, O_RDONLY);
    if (fd1 < 0) { perror("r1"); exit(1); }
    sz1 = read(fd1, file1, MAX_SIZE);
    file1[sz1] = '\0';

    int v1=1;
    for(int i = 0; i < sz1; ++i){
        if(file1[i] == '\n'){
            v1++;
        }
    }

    struct block *block = calloc(1, sizeof(struct block));
    char **verses = calloc(v1, sizeof(char*));
    block->verses = verses;
    block->numberOfVerses = v1;

    int n = 0;
    int beginning = 0;
    for(int i = 0; i <= sz1; ++i){
        if(file1[i] == '\n'|| file1[i] == '\0'){
            char *verse = calloc(i-beginning+1, sizeof(char));
            memcpy(verse, &file1[beginning], i-beginning);
            verse[i-beginning+1] = '\0';
            block->verses[n] = verse;
            n++;
            beginning = i+1;

        }
    }
    array->blocks[array->numberOfBlocks] = block;
    array->numberOfBlocks++;
    free(file1);
    return array->numberOfBlocks-1;
}

void deleteBlock(struct mainArray *array, int index){
    struct block *block = array->blocks[index];
    for(int i = index+1; i<array->numberOfBlocks; ++i){
        array->blocks[i-1] = array->blocks[i];
    }
    array->numberOfBlocks--;
    for(int i = 0; i<block->numberOfVerses; i++){
        free(block->verses[i]);
    }
    free(block);
}

void deleteVerse(struct block *block, int index){
    char *verse = block->verses[index];
    for(int i = index+1; i<block->numberOfVerses; ++i){
        block->verses[i-1] = block->verses[i];
    }
    block->numberOfVerses--;
    free(verse);
}


void freeMemory(struct mainArray *array){
    if (array==NULL)return;;
    for(int i = 0; i< array->numberOfBlocks; ++i){
        if(array->blocks[i]==NULL)break;
        for(int j = 0; j<array->blocks[i]->numberOfVerses; ++j){
            if(array->blocks[i]->verses[j]==NULL)break;
            free(array->blocks[i]->verses[j]);
        }
        free(array->blocks[i]);
    }
    free(array);
}



int getNumberOfVerses(struct block *block){
    return block->numberOfVerses;
}










