#include "control.h"

union semun union_sem;
struct sembuf sem_buffer;

int story_create() {
    // semaphore creation
    int semd = semget(SEMKEY, 1, CREAT_BASE | IPC_EXCL);
    if (semd == -1) {
        printf("Error creating semaphore: %s\n", strerror(errno));
        return -1;
    } else {
        union_sem.val = 1;
        semctl(semd, 0, SETVAL, union_sem);
        printf("Semaphore created successfully\n");
    }
    // shared memory creation
    int shmd = semget(MEMKEY, SIZE, CREAT_BASE);
    if (shmd == -1) {
        printf("Error creating shared memory: %s\n", strerror(errno));
        return -1;
    } else {
        printf("Shared memory created successfully\n");
    }
    // story file creation
    int story_filedes = open(story, O_CREAT | O_TRUNC | O_RDWR, 0644);
    if (story_filedes == -1) {
        printf("Error creating story file: %s\n", strerror(errno));
        return -1;
    } else {
        printf("Story file created successfully\n");
        close(story_filedes);
    }

    return 0;
}

int story_view() {
    FILE * storyfile = fopen(story, "r");
    if (storyfile == NULL) {
        printf("Error opening story file (\"%s\"): %s\n", story, strerror(errno));
        return -1;
    } else {
        printf("The story so far:\n");
        char c;
        while ((c = fgetc(storyfile)) != EOF) {
            fputc(c, stdin);
        }
        fclose(storyfile);
        return 0;
    }
}

int story_remove() {
    // behavior: waits for access, prints contents
    // removes all if and only if semaphore, shared memory segment, and file are accessible

    // get semaphore info
    int semd = semget(SEMKEY, 1, 0);
    if (semd == -1) {
        printf("Error accessing semaphore: %s\n", strerror(errno));
        return -1;
    } else {
        // waits for availability
        printf("trying to get in...\n");
        sem_buffer.sem_num = 0;
        sem_buffer.sem_flg = SEM_UNDO;
        sem_buffer.sem_op = -1;
        semop(semd, &sem_buffer, 1);
    }
    story_view();

    // get shared memory and file info
    int shmd = shmget(MEMKEY, SIZE, 0);
    if (shmd == -1) {
        printf("Error accessing shared memory: %s\n", strerror(errno));
        return -1;
    }
    int story_filedes = open(story, O_RDONLY);
    if (story_filedes == -1) {
        printf("Error accessing story file: %s\n", strerror(errno));
        return -1;
    }

    // removal of all
    shmctl(shmd, IPC_RMID, 0);
    printf("Shared memory segment removed\n");
    semctl(semd, IPC_RMID, 0);
    printf("Semaphore removed\n");
    remove(story);
    printf("Story file removed\n");
}

int main(int argc, char * argv[]) {
    if (argc < 2) {
        printf("Usage: ./control [-crv]\n");
    } else {
        switch(argv[1][1]) {
            case 'c':
                sem_create();
                break;
            case 'r':
                sem_remove();
                break;
            case 'v':
                story_view();
                break;
            default:
                printf("Usage: ./control [-crv]\n");
                break;
        }
    }
}