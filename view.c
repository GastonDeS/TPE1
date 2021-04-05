#include "view.h"

int main(int argc, char const *argv[]) {
    
    if (setvbuf(stdin, NULL, _IONBF, 0) != 0)
        perror("Error Disable buffering");

    char nameShm[100] = {0};
    int sizeShm;
    int fileCount;

    if (argc == 1) {

        char *line = NULL;
        size_t len = 0;

        checkError(getline(&line, &len, stdin),"getting line");
        char *tok = strtok(line, " ");
        strcpy(nameShm, tok);
        tok = strtok(NULL, " ");
        sizeShm = atoi(tok);
        tok = strtok(NULL, " ");
        fileCount = atoi(tok);
        free(line);
    } else if ( argc == 4) {
        strcpy(nameShm, argv[1]);
        sizeShm = atoi(argv[2]);
        fileCount = atoi(argv[3]);
    } else {
        printf("bad argument count\n");
        exit(EXIT_FAILURE);
    }
    if (sizeShm <= 0) {
        perror("passed memory size");
        exit(-1);
    }

    int fdShm = shm_open(nameShm, O_CREAT | O_RDWR, 0666);
    checkError(fdShm,"shmopen");

    void * sharedMemory = mmap(NULL, sizeShm, PROT_READ | PROT_WRITE, MAP_SHARED, fdShm, 0);
    checkErrno(sharedMemory,"shared memory map",MAP_FAILED);

    sem_t *semShm = sem_open("semShm", O_CREAT, 0600, 0);
    checkErrno(semShm,"semaphore",SEM_FAILED);

    //char* shOriginal = (char *) sharedMemory;
    char* shIndex = (char *) sharedMemory;

    //el while para espear y leer de emoria compartida
    int readsCount =0;
    while (readsCount < fileCount) {

        checkError(sem_wait(semShm),"waiting semaphore");
        printf("%s\n", shIndex);

        shIndex += STEP_SHM;
        readsCount++;


    }
    return 0;
}
