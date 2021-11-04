#include "local.h"

void readFromQueue(int);
FILE *fptr_queue_IDs;

void main(int argc, char *argv[])
{
    printf("officer %d\n\n", getpid());
    FILE *fptr_queue_IDs;
    int num_officers = atoi(argv[1]);
    int queue_IDs[num_officers];
    MESSAGE message;
    struct passanger passngr_struct;
    int n;
    char *shmptr;
    static struct MEMORY *memptr;
    int index = 0;
    int semid, shmid;
    pid_t ppid = getppid();

    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<read the queue IDs>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    if ((fptr_queue_IDs = fopen(QUEUE_IDs_FILE, "r")) == NULL)
    {
        perror("CHILD1_FILE");
        exit(-7);
    }

    for (int f = 0; f < num_officers; f++)
    {
        fscanf(fptr_queue_IDs, "%d\n", &queue_IDs[f]);
    }

    fclose(fptr_queue_IDs);
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< attach to shared memory >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    // no ipc-create: I just want to connect to it
    if ((shmid = shmget((int)ppid, 0, 0)) != -1)
    {
        printf("shmid = %d \n", shmid);
        if ((shmptr = (char *)shmat(shmid, (char *)0, 0)) == (char *)-1)
        {
            perror("shmat -- passenger -- attach");
            exit(1);
        }
        memptr = (struct MEMORY *)shmptr;
    }
    else
    {
        perror("shmget -- passenger-- access");
        exit(2);
    }
    /*
   * Access the semaphore set
   */
    //
    if ((semid = semget((int)ppid, 2, 0)) == -1)
    {
        perror("semget -- producer -- access");
        exit(3);
    }

    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<,, officers are at the crossing points doing their job>>>>>>>>>>>>>>>>>>>>>>>
    if (strcmp(argv[2], "P") == 0)
    {

        printf("P: ");
        if ((n = msgrcv(queue_IDs[atoi(argv[3])], (char *)&passngr_struct, sizeof(passngr_struct), 0, 0)) != -1)
        {
            if (passngr_struct.havepassport == 1)
            {
                printf("inside P:\n");
                printf("Palestinian: passenger ID = %d and nationality = %s\n", passngr_struct.pid, passngr_struct.nationality);

                acquire.sem_num = AVAIL_SLOTS; // decremnt a semphore from the 6 semaphore
                printf("Passenger: acquire.sem_num =  %d and AVAIL_SLOTS = %d\n\n", acquire.sem_num, AVAIL_SLOTS);
                if (semop(semid, &acquire, 1) == -1)
                {
                    perror("semop -- passenger -- acquire");
                    exit(-3);
                }
                memptr->passengers_IDs[index] = getpid();
                printf("passenger %d has entered the memory:  \n", memptr->passengers_IDs[index]);
            
                index++;
                release.sem_num = TO_CONSUME;
            }
            else
            {
                printf("The Palestinian passenger with ID = %d is denied crossing\n", passngr_struct.pid);
                kill(passngr_struct.pid, SIGTERM);
            }
        }
    }
    else if (strcmp(argv[2], "J") == 0)
    {

        printf("J: ");
        if ((n = msgrcv(queue_IDs[atoi(argv[3])], (char *)&passngr_struct, sizeof(passngr_struct), 0, 0)) != -1)
        {
            if (passngr_struct.havepassport == 1)
            {
                printf("inside j: \n");
                printf("Jordanian: passenger ID = %d and nationality = %s\n", passngr_struct.pid, passngr_struct.nationality);
                acquire.sem_num = AVAIL_SLOTS; // decremnt a semphore from the 6 semaphore
                printf("Passenger: acquire.sem_num =  %d and AVAIL_SLOTS = %d\n\n", acquire.sem_num, AVAIL_SLOTS);
                if (semop(semid, &acquire, 1) == -1)
                {
                    perror("semop -- passenger -- acquire");
                    exit(-3);
                }
                memptr->passengers_IDs[index] = getpid();
                printf("passenger %d has entered the memory:  \n", memptr->passengers_IDs[index]);
              
                index++;
                release.sem_num = TO_CONSUME;
            }
            else
            {
                printf("The Jordanian passenger with ID = %d is denied crossing\n", passngr_struct.pid);
                kill(passngr_struct.pid, SIGTERM);
            }
        }
    }
    else if (strcmp(argv[2], "F") == 0)
    {

        printf("F: ");
        if ((n = msgrcv(queue_IDs[atoi(argv[3])], (char *)&passngr_struct, sizeof(passngr_struct), 0, 0)) != -1)
        {
            if (passngr_struct.havepassport == 1)
            {

                printf("inside k:\n");
                printf("Forign: passenger ID = %d and nationality = %s\n", passngr_struct.pid, passngr_struct.nationality);
                acquire.sem_num = AVAIL_SLOTS; // decremnt a semphore from the 6 semaphore

                printf("Passenger: acquire.sem_num =  %d and AVAIL_SLOTS = %d\n\n", acquire.sem_num, AVAIL_SLOTS);
                if (semop(semid, &acquire, 1) == -1)
                {
                    perror("semop -- passenger -- acquire");
                    exit(-3);
                }
                memptr->passengers_IDs[index] = getpid();
                printf("passenger %d has entered the memory:  \n", getpid());
                printf("%d \n ", memptr->passengers_IDs[index]);
                index++;
                release.sem_num = TO_CONSUME;
            }
            else
            {
                printf("The Foriegner passenger with ID = %d is denied crossing\n", passngr_struct.pid);
                kill(passngr_struct.pid, SIGTERM);
            }
        }
    }

    while (1)
        ;
}
// void readFromQueue(int the_sig)
// {
// }