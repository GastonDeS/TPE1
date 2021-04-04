#include <strings.h>
#include <string.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <sys/stat.h> 
#include <fcntl.h> 
#include <stdio.h>
#include <semaphore.h>
#include <sys/shm.h>

int main(int argc, char const *argv[]) {

    //1. conseguir los datos por args o standard
    //2. hacer el mmap para mapearlo aca tmb 
    //3. hacer el ciclo de lectura e impresion
    /*while (1) {
        wait(sem);
        read();
        ...
    }*/

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

    void * sharedMemory = mmap(0, 2000, PROT_READ | PROT_WRITE, MAP_SHARED, fdShm, 0);
    if (sharedMemory == MAP_FAILED) {
        perror("shared memory map");
        exit(-1);
    }

    sem_t *semShm = sem_open("semShm", O_CREAT, 0700, 0);
    if (semShm == SEM_FAILED){
        perror("semaphore");
        exit(-1);
    }

    char *charShm = (char *) sharedMemory;
    //el while para espear y leer de emoria compartida
    while (1) {
        if (sem_wait(semShm) == -1) {
            perror("waiting semaphore");
            exit(-1);
        }
        printf("%s\n", charShm);
        charShm +=512;
    }
    
}
