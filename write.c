#include "write.h"

struct sembuf sem_buffer;

int story_write() {
    int semd = semget(SEMKEY, 1, 0);
    if (semd == -1) {
        printf("Error accessing semaphore: %s\n", strerror(errno));
        return -1;
    }
    printf("trying to get in\n");
    semop(semd, &sem_buffer, 1);

    // get line size
    int shmd = shmget(SHMKEY, SIZE, 0);
    if (shmd == -1) {
        printf("Error accessing shared memory: %s\n", strerror(errno));
        return -1;
    }
    short * size_ptr = shmat(shmd, 0, 0);
    short size = *size_ptr;
    // read from file
    int story_filedes = open(story, O_RDWR | O_APPEND);
    if (shmd == -1) {
        printf("Error opening story file: %s\n", strerror(errno));
        return -1;
    }

    shmdt(size_ptr);
    close(story_filedes);
    sem_buffer.sem_op = 1;
    semop(semd, &sem_buffer, 1);
}

int main() {
    
}