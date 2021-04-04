#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h> 
#include <fcntl.h> 
#include <stdio.h>
#include <semaphore.h>
#include <sys/shm.h>

#define SHM_STEP 200

int main(int argc, char const *argv[]) {


    if (setvbuf(stdin, NULL, _IONBF, 0) != 0)
        perror("Error Disable buffering");

    char nameShm[100] = {0};
    int sizeShm;
    
    if (argc == 1) {
        
        char *line = NULL;
        size_t len = 0;
        
        if (getline(&line, &len, stdin) == -1) {
            perror("getting line");
            exit(-1);
            }
        char *tok = strtok(line, " ");
        strcpy(nameShm, tok);
        tok = strtok(NULL, " ");
        sizeShm = atoi(tok);
        free(line);
    }

    else {
        strcpy(nameShm, argv[1]);

        sizeShm = atoi(argv[2]);
    }

    if (sizeShm <= 0) {
        
        perror("passed memory size");
        exit(-1);
    }
    
    int fdShm = shm_open(nameShm, O_CREAT | O_RDWR, 0666);
    if (fdShm == -1)
    {
        perror("shmopen");
        exit(-1);
    }

    void * sharedMemory = mmap(NULL, sizeShm, PROT_READ, MAP_SHARED, fdShm, 0);
    if (sharedMemory == MAP_FAILED) {
        perror("shared memory map");
        exit(-1);
    }

    sem_t *semShm = sem_open("semShm", O_CREAT, 0700, 0);
    if (semShm == SEM_FAILED){
        perror("semaphore");
        exit(-1);
    }

    //char* shOriginal = (char *) sharedMemory;
    char* shIndex = (char *) sharedMemory;

    //el while para espear y leer de emoria compartida
    while (1) {
        
        if (sem_wait(semShm) == -1) {
            perror("waiting semaphore");
            exit(-1);
        }

        printf("%s\n", shIndex);

    }   
}
