#include <strings.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h> 
#include <fcntl.h> 
#include <stdio.h>
#include <semaphore.h>

int main(int argc, char const *argv[]) {

    //1. conseguir los datos por args o standard
    //2. hacer el mmap para mapearlo aca tmb 
    //3. hacer el ciclo de lectura e impresion
    /*while (1) {
        wait(sem);
        read();
        ...
    }*/



    char nameShm[20];
    int sizeShm;

     if (setvbuf(stdin, NULL, _IONBF, 0) != 0)
        throwError("Error Disable buffering");

    if (argc == 1) {
        char *line = NULL;
        size_t size;
        if (getline(&line, &size, stdin) == -1)
            throwError("getting line");
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
    }
    
    int fdShm = shm_open(nameShm, O_CREAT | O_RDWR, 666);
    if (fdShm == -1)
    {
        perror("shmopen");
    }

    void * sharedMemory = mmap(0, sizeShm, PROT_READ | PROT_WRITE, MAP_SHARED, fdShm, 0);
    if (sharedMemory == MAP_FAILED) {
        perror("shared memory map");
    }

    sem_t *semShm = sem_open("semShm", O_CREAT, 700, 0);
    if (semShm == SEM_FAILED){
        perror("semaphore");
    }
}
