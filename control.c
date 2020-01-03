#include "control.h"

union semun union_sem;
struct sembuf sem_buffer;

int story_create() {
    // semaphore creation
    int semd = semget(SEMKEY, 1, IPC_CREAT | IPC_EXCL | 0777);
    if (semd == -1) {
        printf("Error creating semaphore: %s\n", strerror(errno));
        
    }
    union_sem.val = 1;
    semctl(semd, 0, SETVAL, union_sem);
    printf("Semaphore created successfully\n");

    // shared memory creation
    int shmd = shmget(SHMKEY, SIZE, IPC_CREAT | 0777);
    if (shmd == -1) {
        printf("Error creating shared memory: %s\n", strerror(errno));
        
    }
    printf("Shared memory created successfully\n");

    // story file creation
    int story_filedes = open(story, O_CREAT | O_TRUNC | O_RDWR, 0777);
    if (story_filedes == -1) {
        printf("Error creating story file: %s\n", strerror(errno));
        
    }
    printf("Story file created successfully\n");
    close(story_filedes);

    return 0;
}

int story_view() {
    FILE * storyfile = fopen(story, "r");
    if (storyfile == NULL) {
        printf("Error opening story file (\"%s\"): %s\n", story, strerror(errno));
        
    }
    printf("The story so far:\n");
    char c;
    while ((c = fgetc(storyfile)) != EOF) {
        fputc(c, stdout);
    }
    fclose(storyfile);
    return 0;
}

int story_remove() {
    // behavior: prints contents, waits for memory access
    // removes all if and only if semaphore, shared memory segment, and file are accessible

    story_view();
    // get semaphore info, wait for access
    int semd = semget(SEMKEY, 1, 0);
    if (semd == -1) {
        printf("Error accessing semaphore: %s\n", strerror(errno));
        
    }
    // waits for availability
    printf("awaiting access\n");
    sem_buffer.sem_num = 0;
    sem_buffer.sem_flg = SEM_UNDO;
    sem_buffer.sem_op = -1;
    semop(semd, &sem_buffer, 1);

    // get shared memory and file info
    int shmd = shmget(SHMKEY, SIZE, 0);
    if (shmd == -1) {
        printf("Error accessing shared memory: %s\n", strerror(errno));
        
    }
    int story_filedes = open(story, O_RDONLY);
    if (story_filedes == -1) {
        printf("Error accessing story file: %s\n", strerror(errno));
        
    }

    // removal of all
    shmctl(shmd, IPC_RMID, 0);
    printf("Shared memory segment removed\n");
    remove(story);
    printf("Story file removed\n");
    semctl(semd, IPC_RMID, 0);
    printf("Semaphore removed\n");
}

int main(int argc, char * argv[]) {
    if (argc != 2) {
        printf("Usage: ./control [-crv]\n");
    } else {
        switch(argv[1][1]) {
            case 'c': story_create();
                break;
            case 'r': story_remove();
                break;
            case 'v': story_view();
                break;
            default:
                printf("Usage: ./control [-crv]\n");
                break;
        }
    }
}