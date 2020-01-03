#include "write.h"

struct sembuf sem_buffer;

int story_write() {
    int semd = semget(SEMKEY, 1, 0);
    if (semd == -1) {
        printf("Error accessing semaphore: %s\n", strerror(errno));
    }
    printf("\nawaiting access\n");
    sem_buffer.sem_op = -1;
    semop(semd, &sem_buffer, 1);

    // get line size
    int shmd = shmget(SHMKEY, SIZE, 0);
    if (shmd == -1) {
        printf("Error accessing shared memory: %s\n", strerror(errno));
    }
    int * size_ptr = shmat(shmd, 0, 0);
    int size = *size_ptr;

    // read from file
    int story_filedes = open(story, O_RDONLY);
    if (story_filedes == -1) {
        printf("Error opening story file: %s\n", strerror(errno));
    }

    char * linebuffer = calloc(size, 1);
    printf("Last line of story:\n");
    lseek(story_filedes, -1 * size, SEEK_END);
    read(story_filedes, linebuffer, size);
    printf("%s", linebuffer);
    close(story_filedes);
    char inbuffer[1024];
    printf("Enter next line:\n");
    fgets(inbuffer, 1024, stdin);
    story_filedes = open(story, O_WRONLY | O_APPEND);
    if (story_filedes == -1) {
        printf("Error opening story file: %s\n", strerror(errno));
    }
    *size_ptr = strlen(inbuffer);
    write(story_filedes, inbuffer, *size_ptr);

    //free, detach, close file, release semaphore
    free(linebuffer);
    shmdt(size_ptr);
    close(story_filedes);
    sem_buffer.sem_op = 1;
    semop(semd, &sem_buffer, 1);
}

int main() {
    story_write();
}