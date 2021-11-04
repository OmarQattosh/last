#ifndef __LOCAL_H__
#define __LOCAL_H__

/*
 * Common header file for Message Queue Example
 */

#include <stdio.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <string.h>
#include <ctype.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <time.h>
#include <sys/sem.h>
#include <sys/shm.h>
#include <wait.h>

#define INPUT_FILE "./input.txt"
#define QUEUE_IDs_FILE "./queue_IDs.txt"

#define pass_struct_slots 100
union semun
{
  int val;
  struct semid_ds *buf;
  ushort *array;
};

struct sembuf acquire = {0, -1, SEM_UNDO},
              release = {0, 1, SEM_UNDO};

enum
{
  AVAIL_SLOTS,
  TO_CONSUME
};

struct passanger
{
  int pid;
  char nationality[2];
  int havepassport;
  int P_lvl;
};

struct MEMORY
{
  // char buffer[N_SLOTS][SLOT_LEN];
  int head, tail;
  int passengers_IDs[pass_struct_slots];
  // struct passanger passenger_struct [pass_struct_slots][50];
  // int pid [pass_struct_slots][10];
  // char nationality [pass_struct_slots][2]; 
  // int havepassport [pass_struct_slots][5];
  // int P_lvl [pass_struct_slots][5];
};

typedef struct
{
  // long msg_to; /* Placed in the queue for */
  // long msg_fm; /* Placed in the queue by  */
  long msg_type;
  char buffer[BUFSIZ];
} MESSAGE;

#endif
