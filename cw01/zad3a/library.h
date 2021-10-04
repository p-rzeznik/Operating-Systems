#ifndef LAB1_LIBRARY_H
#define LAB1_LIBRARY_H

struct pair{
    char *first;
    char *second;
};


struct fileSequence{
    int numberOfSequences;
    struct pair *fileSequence;
};

struct block{
    int numberOfVerses;
    char **verses;
};

struct mainArray{
    int numberOfBlocks;
    struct block **blocks;
};

struct mainArray *createArray(int numberOfBlocks);

struct block *mergePair(struct pair files);

void mergeAll(struct mainArray *array, struct fileSequence files);

void saveToFile(struct block *block, char * path);

void deleteVerse(struct block *block, int index);

void deleteBlock(struct mainArray *array, int index);

int loadFromFile(struct mainArray *array, char * path);

int getNumberOfVerses(struct block *block);

void printMergedFiles(struct mainArray *array);

void freeMemory(struct mainArray *array);

#endif //LAB1_LIBRARY_H
