
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <sys/shm.h>
#include <sys/ipc.h>
#include <sys/types.h>
#include <sys/sem.h>

#define SEMKEY 6674 //Gravitational constant
#define MEMKEY 6626 //Planck constant
#define SIZE 1024

int CREAT_BASE = IPC_CREAT | IPC_EXCL | 0644;

union semun {
	int val;		// Value for SETVAL
	struct semid_ds *buf;		//Buffer for IPC_STAT, IPC_SET
	unsigned short *array;	//Array for GETALL, SETALL
	struct seminfo *__buf;	//Buffer for IPC_INFO (Linux-specific)
};

int story_create();
int story_remove();
int story_view();