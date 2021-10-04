#include <stdio.h>
#include <dirent.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_SIZE 100000000


void search_dir(char *path, char * pattern, int depth, int max_depth);

int main(int argc, char **argv) { // path to start dir, pattern, max_depth
    if (argc != 4){
        exit(-1);
    }
    search_dir(argv[1], argv[2], 0, (int)strtol(argv[3], NULL, 10));

    return 0;
}

void search_dir(char *path, char * pattern, int depth,  int max_depth){
    if(depth >= max_depth){
        return;
    }

    DIR *dp = opendir(path);
    struct dirent *dirp;

    char new_path[PATH_MAX];

    while((dirp = readdir(dp)) != NULL){
        if(strcmp(dirp->d_name, ".")==0 || strcmp(dirp->d_name, "..")==0){
            continue;
        }

        if(dirp->d_type==DT_REG){
            char filepath[PATH_MAX];
            strcpy(filepath, path);
            strcat(filepath, "/");
            strcat(filepath, dirp->d_name);
            FILE *fp = fopen(filepath, "r");
            char *buf = calloc(MAX_SIZE, sizeof(char));
            fread(buf, sizeof(char), MAX_SIZE, fp);
            if(strstr(buf, pattern)!=NULL){
                printf("Path: %s   PID: %d    Filename: %s\n", path, getpid(), dirp->d_name);
            }
            free(buf);
            fclose(fp);
        } else if (dirp->d_type==DT_DIR){
            strcpy(new_path, path);
            strcat(new_path, "/");
            strcat(new_path, dirp->d_name);
            pid_t child_pid;
            if((child_pid = fork())==0){
                search_dir(new_path, pattern, depth+1, max_depth);
                exit(0);
            }
        }
    }

    closedir(dp);


}

