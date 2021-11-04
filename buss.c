
#include "local.h"

void main(int argc, char *argv[])
{
    printf("in bus:\n");
    int semid, shmid;
    pid_t ppid = getppid();
    char *shmptr;
    struct MEMORY *memptr;
    int index = 0;
    index++;
    printf("index = %d ", index);

    if ((shmid = shmget((int)ppid, 0, 0)) != -1)
    {
        if ((shmptr = (char *)shmat(shmid, (char *)0, 0)) == (char *)-1)
        {
            perror("shmat -- bus -- attach");
            exit(1);
        }
        memptr = (struct MEMORY *)shmptr;
    }
    else
    {
        perror("shmget -- bus -- access");
        exit(2);
    }

    /*
   * Access the semaphore set
   */
    if ((semid = semget((int)ppid, 2, 0)) == -1)
    {
        perror("semget -- bus -- access");
        exit(3);
    }

    acquire.sem_num = TO_CONSUME;
    printf("buss:  acquire.sem_num = %d and TO_CONSUME = %d\n\n", acquire.sem_num, TO_CONSUME);

    if (semop(semid, &acquire, 1) == -1)
    {
        perror("semop -- bus -- acquire");
        exit(4);
    }
    printf("in buss after aquie\n:");
    printf("in buss: %d \n ", memptr->passengers_IDs[0]);

    release.sem_num = AVAIL_SLOTS;
    printf("buss: release.sem_num = %d and AVAIL_SLOTS = %d\n\n", release.sem_num, AVAIL_SLOTS);

    if (semop(semid, &release, 1) == -1)
    {
        perror("semop -- bus-- release");
        exit(5);
    }

    printf("<<<<<<<<<<<<<<<<<<<<<<<<<<>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>\n");

    while (1)
        ;
}