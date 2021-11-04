#include "local.h"

int havePassportFunc();

void main(int argc, char *argv[])
{

    int semid, shmid, croaker;
    char *shmptr;
    pid_t p_id, c_id, pid = getpid();
    FILE *fptr_input;
    FILE *fptr_queue_IDs;
    MESSAGE message;
    pid_t pid_passengers, pid_officers, pid_busses, pid_arr[3];
    static struct MEMORY memory;
    union semun arg;

    //<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<read the input file>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    char inp_file_values[25][40];

    if ((fptr_input = fopen(INPUT_FILE, "r")) == NULL)
    {
        perror("INPUT_FILE");
        exit(-1);
    }

    // read the file
    for (int i = 0; i < 25; i++)
    {
        fscanf(fptr_input, "%s\n", inp_file_values[i]);
        printf("file_values[%d] = %s  \n", i, inp_file_values[i]);
    }
    int numCrossPointsP = atoi(inp_file_values[7]);
    int numCrossPointsJ = atoi(inp_file_values[9]);
    int numCrossPointsF = atoi(inp_file_values[11]);
    int numOfOfficers = atoi(inp_file_values[13]);
    char pidOfficerssAsStr[numOfOfficers];

    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< create message queue >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>...
    //  message queue is created
    int mid[numOfOfficers]; // queue for each officer
    char u_char = 'A';
    int key;
    for (int l = 0; l < numOfOfficers; l++, u_char++)
    {
        key = ftok(".", u_char);
        // conect to message queue if exists
        if ((mid[l] = msgget(key, 0)) == -1)
        {

            // create message queue
            if ((mid[l] = msgget(key, IPC_CREAT | 0666)) == -1)
            {
                perror("Queue create");
                exit(-5);
            }
        }
    }
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< write message queue IDs on a file >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>

    if ((fptr_queue_IDs = fopen(QUEUE_IDs_FILE, "w")) == NULL)
    {
        perror("QUEUE_IDs_FILE");
        exit(-3);
    }
    for (int i = 0; i < numOfOfficers; i++)
    {
        fprintf(fptr_queue_IDs, "%d\n", mid[i]);
    }
    fclose(fptr_queue_IDs); //close the file

    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< create shared memory & semaphores >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    static ushort start_val[2] = {9, 0};
    // for (int i = 0; i < numOfOfficers; i++)
    // {
    //     if (i == 0)
    //         start_val[i] = 1;
    //     start_val[i] = 0;
    // }

    if ((shmid = shmget((int)pid, sizeof(memory),
                        IPC_CREAT | 0660)) != -1)
    {
        if ((shmptr = (char *)shmat(shmid, 0, 0)) == (char *)-1) // 0: means start from 0
        {
            perror("shmptr -- parent -- attach");
            exit(-3);
        }
        memcpy(shmptr, (char *)&memory, sizeof(memory));
    }
    else
    {
        perror("shmid -- parent -- creation");
        exit(-22);
    }

    /*
   * Create and initialize the semaphores
   */
    if ((semid = semget((int)pid, 2,
                        IPC_CREAT | 0666)) != -1)
    {
        arg.array = start_val;

        if (semctl(semid, 0, SETALL, arg) == -1)
        {
            perror("semctl -- parent -- initialization");
            exit(-10);
        }
    }
    else
    {
        perror("semget -- parent -- creation");
        exit(-44);
    }

    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<< fork the busses >>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    //    inp_file_values[15] is number of busses
    for (int b = 0; b < atoi(inp_file_values[15]); b++)
    {
        pid_busses = fork();
        if (pid_busses == 0)
        {
            if (execl("./buss", "buss", NULL) == -1)
            {
                perror("EXECL: officers");
                exit(-2);
            }
        }
    }

    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<fork the passengers based on their percentages>>>>>>>>>>>>>>>>>>>>>>
    // while (1)
    {
        // srand(time(0));
        int num_of_pass = 4; //(int)(1 + rand() % 10);
        int numPal = num_of_pass * atof(inp_file_values[1]);
        int numJor = num_of_pass * atof(inp_file_values[3]);
        int numFor = num_of_pass - numPal - numJor;
        char midAsStr[10];

        printf("num of passengers = %d\n", num_of_pass);
        for (int i = 0; i < num_of_pass; i++)
        {
            // fork the passengers
            pid_passengers = fork();

            // Palestinian passenger
            if (pid_passengers == 0 && i < numPal)
            {
                sprintf(midAsStr, "%d", mid[i % numCrossPointsP]);
                if (execl("./passengers", "passengers", "P", midAsStr, NULL) == -1)
                {
                    perror("EXECL: Passengers");
                    exit(-2);
                }
            }
            //Jordanians
            else if (pid_passengers == 0 && (i >= numPal && i < (num_of_pass - numFor)))
            {
                sprintf(midAsStr, "%d", mid[(i % numCrossPointsJ) + numCrossPointsP]);

                if (execl("./passengers", "passengers", "J", midAsStr, NULL) == -1)
                {
                    perror("EXECL: Passengers");
                    exit(-3);
                }
            }
            // Foriegners
            else if (pid_passengers == 0)
            {
                sprintf(midAsStr, "%d", mid[(i % numCrossPointsF) + numCrossPointsP + numCrossPointsJ]);

                if (execl("./passengers", "passengers", "F", midAsStr, NULL) == -1)
                {
                    perror("EXECL: Passengers");
                    exit(-4);
                }
            }
        }

        // sleep(90);
    }
    sleep(10);
    // <<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<<forking the officers>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>
    int officers_id[numOfOfficers];
    char numOfficers_str[10];
    int crossP = atoi(inp_file_values[7]);
    int crossJ = atoi(inp_file_values[9]);
    int crossF = atoi(inp_file_values[11]);

    int numPalCross = crossP;
    int numJorCross = crossJ;
    int numForCross = numOfOfficers - numPalCross - numJorCross;
    char index[10];

    for (int k = 0; k < numOfOfficers; k++)
    {
        pid_officers = fork();
        if (pid_officers == 0 && k < numPalCross)
        {
            sprintf(numOfficers_str, "%d", numOfOfficers);
            sprintf(index, "%d", k);
            if (execl("./officers", "officers", numOfficers_str, "P", index, NULL) == -1)
            {
                perror("EXECL: officers");
                exit(-2);
            }
        }
        else if (pid_officers == 0 && (k >= numPalCross && k < (numOfOfficers - numForCross)))
        {
            sprintf(numOfficers_str, "%d", numOfOfficers);
            sprintf(index, "%d", k);
            if (execl("./officers", "officers", numOfficers_str, "J", index, NULL) == -1)
            {
                perror("EXECL: officers");
                exit(-3);
            }
        }
        else if (pid_officers == 0)
        {
            sprintf(numOfficers_str, "%d", numOfOfficers);
            sprintf(index, "%d", k);
            if (execl("./officers", "officers", numOfficers_str, "F", index, NULL) == -1)
            {
                perror("EXECL: officers");
                exit(-4);
            }
        }

        sleep(1);
    }
}
