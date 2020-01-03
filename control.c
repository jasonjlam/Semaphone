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

union semun {
               int              val;    /* Value for SETVAL */
               struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
               unsigned short  *array;  /* Array for GETALL, SETALL */
               struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                           (Linux-specific) */
           };
void readStory() {
    int fd;
    fd = open("story", O_RDONLY);
    if (fd == -1) {
        printf("error %d: %s\n", errno, strerror(errno));
        return;
    }
    printf("The story so far:\n");
    char buffer[256];
    while(read(fd, buffer, 256) > 0) {
        printf("%s\n", buffer);
    }
    close(fd);

}
int main(int argc, char * argv[]) {
    int sem;
    int shmid;
    int semid;
    if (argc == 2) {
        if(strcmp(argv[1], "-c") == 0) {
            semid = semget(25565, 1, IPC_EXCL | IPC_CREAT| 0644);
            if(semid == -1) {
                semid = semget(25565, 1, 0);
                sem = semctl(semid, 0, GETVAL, 0);
            }
            else {
                union semun us;
                us.val = 1;
                sem = semctl(semid, 0, SETVAL, us);
            }
            printf("Created semaphore \n");
            shmid = shmget(25566, 256, IPC_CREAT | 0644);
            printf("Created shared memory \n");
            int fd = open("story", O_CREAT | O_EXCL | O_TRUNC, 0644);
            if (fd == -1) {
                fd = open("story", O_TRUNC);
            }
            close(fd);
            printf("Created story \n");
        }
        else if (strcmp(argv[1], "-r") == 0) {
            semid = semget(25565, 1, 0);
            struct sembuf sembuffer;
            sembuffer.sem_num = 0;
            sembuffer.sem_op = -1;
            printf("Trying to get in\n");
            semop(semid, &sembuffer, 1);
            readStory();
            int shmid = shmget(25566, 256, 0644);
            if (remove("story") == 0) {
                 printf("Removed story \n");
            }
            semctl(semid, IPC_RMID, 0);
            printf("Removed semaphore \n");
            shmctl(shmid, IPC_RMID, 0);
            printf("Removed shared memory \n");
        }
        else if (strcmp(argv[1],"-v") == 0 ) {
            readStory();
        }
    }
    else {
        printf("Error: Not a valid tag and/or wrong number of arguments!\n" );
    }

    return 0;
}
