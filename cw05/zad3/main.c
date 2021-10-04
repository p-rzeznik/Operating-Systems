#include <unistd.h>
#include <string.h>
#include <sys/stat.h>
#include <wait.h>


int main(){ 

	mkfifo("fifo", S_IRUSR | S_IWUSR);

	pid_t pid_tab[6];
    if ((pid_tab[0] = fork()) == 0)
        execlp("./consumer", "./consumer", "fifo", "output.txt", "5", NULL);

    if ((pid_tab[1] = fork()) == 0)
        execlp("./producer", "./producer", "fifo", "3", "A.txt", "5", NULL);

    if ((pid_tab[2] = fork()) == 0)
        execlp("./producer", "./producer", "fifo", "1", "B.txt", "5", NULL);

    if ((pid_tab[3] = fork()) == 0)
        execlp("./producer", "./producer", "fifo", "4", "C.txt", "5", NULL);

    if ((pid_tab[4] = fork()) == 0)
        execlp("./producer", "./producer", "fifo", "2", "D.txt", "5", NULL);

    if ((pid_tab[5] = fork()) == 0)
        execlp("./producer", "./producer", "fifo", "5", "E.txt", "5", NULL);

    for (int i = 0; i < 6; i++)
        waitpid(pid_tab[i], NULL, 0);

    return 0;

}