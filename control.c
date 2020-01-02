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