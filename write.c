#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <fcntl.h>
#include <errno.h>

int main() {
    int semid;
    int shmid;
    semid = semget(25565, 1, 0);
    struct sembuf sembuffer;
    sembuffer.sem_num = 0;
    sembuffer.sem_op = -1;
    sembuffer.sem_flg = SEM_UNDO;
    printf("trying to get in\n");
    semop(semid, &sembuffer, 1);

    char * line;

    shmid = shmget(25566, 256, 0644);
    if(shmid == -1) {
        printf("error %d: %s\n", errno, strerror(errno));
        return 1;
    }

    line = shmat(shmid, 0, 0);
    printf("Last addition: %s\n", line);

    printf("Your addition: ");
    fgets(line, 256, stdin);
    *strchr(line, '\n') = 0;

    int fd = open("story", O_WRONLY | O_APPEND);
    write(fd, line, 256);
    close(fd);

    shmdt(line);

    return 0;
}
