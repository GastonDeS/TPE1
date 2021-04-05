#include "solve.h"

int main(int argc, char const *argv[]){

    if (argc == 1) {
        printf("bad argument count \n");
        return 1;
    }
    int child= NUM_CHILD;
    if (NUM_CHILD > argc -1) {
        child = argc -1;
    }
    int pathsIni = ((argc-1)/(5*child) < MIN_PATHS_INI)? MIN_PATHS_INI :  (argc-1)/(5*child);
    pathsIni *= child ;

    int fd[child][2];
    off_t sizeShm = STEP_SHM*(argc-1);
    int fdShm;
    void * pntShm;
    void* pntShmIni;

    FILE* result=fopen(RESULT_FILE_NAME, "w");
    checkErrno(result, "open result file", NULL);

    if(setvbuf(stdout, NULL, _IONBF, 0))
        perror("Error setvbuf");

    pntShm = initShM(SHM_NAME, &fdShm, &sizeShm);
    pntShmIni = pntShm;

    sem_t *semShm = sem_open("semShm", O_CREAT, 0600, 0);
    checkErrno(semShm, "sem_open", SEM_FAILED);

    printf("%s %d %d\n", SHM_NAME, (int)sizeShm,argc-1); //impreimo datos necesarios para view
    sleep(2);

    initSlave(child, fd, SLAVE_PATH, NULL);

    //asignacion de la primera tanda de archivos
    int fileSentCount = 0;
    int i;
    int countSlaveFiles[child];
    for ( i = 0; i < child; i++) {
        countSlaveFiles[i] = 0;
    }
    for (i = 0; i < pathsIni; i++){
        char path[4096]={0};
        strcat(path,argv[i+1]);
        strcat(path,"\n");

        checkError(write(fd[i%child][WRITE],path ,strlen(path)),"write");
        fileSentCount++;
        countSlaveFiles[i%child]++;
    }
    int fileRecivedCount = 0;

    //loop
    while (fileRecivedCount < (argc-1)){ // mientras haya cnf para analizar
        fd_set readfds;
        FD_ZERO(&readfds);
        int i,max=0;
        for (i = 0; i < child; i++){
            if (fd[i][READ] > 0) {
                max = (fd[i][READ]>max)?fd[i][READ] : max;
                FD_SET(fd[i][READ], &readfds);
            }
        }
        int ready = select(max+1, &readfds, NULL, NULL, NULL);
        checkError(ready,"select");

        for (i = 0; i < child && ready > 0; i++){
            if(FD_ISSET(fd[i][READ], &readfds)){

                //recibo un file
                char buff[512]={0};
                if(read(fd[i][READ], buff, sizeof(buff)) <= 0){
                    fd[i][READ] = -1;//el hijo termino
                    continue;
                }
                fileRecivedCount++;

                //escribir en result.txt
                fprintf(result,"%s \n", buff);

                if(fileRecivedCount > (argc-(pathsIni-child)+1)){ 
                    fd[i][READ] = -1;
                }

                //envio respuesta el viewer
                checkError(sprintf((char *)(pntShm),"%s \n", buff),"sprint");
                checkError(sem_post(semShm),"post sem"); 
                pntShm += STEP_SHM;
                countSlaveFiles[i]--;
                //envio tasks
                if (countSlaveFiles[i]==0) {
                    if (fileSentCount < argc-1) {
                        char path[4096]={0};
                        char *vec;
                        vec = strcat(path,argv[fileSentCount+1]);
                        vec = strcat(vec,"\n");

                        checkError(write(fd[i][WRITE],vec ,strlen(vec)),"write_sendTasks");
                        fileSentCount++;
                        countSlaveFiles[i]++;
                    }
                }
                
                
                ready--;
            }
        }
    }
    checkError(fclose(result),"close_result.txt");
    checkError(munmap(pntShmIni,sizeShm),"munmap");
    checkError(close(fdShm)," fd colse");
    checkError(sem_close(semShm),"sem_close");

    return 0;
}

void* initShM(char* const name, int* fdShm, off_t* sizeShm){ 
    checkError( ((*fdShm)=shm_open(name, O_CREAT | O_RDWR, 0666)) , "shmopen");
    checkError(ftruncate(*fdShm, *sizeShm),"ftrunate" );
    void* pntShm = mmap(NULL, *sizeShm, PROT_WRITE, MAP_SHARED, *fdShm, 0);
    checkErrno(pntShm , "SHM_map", MAP_FAILED);
    return pntShm;
}

void initSlave(int slaveNum, int fd[][2], const char *path, char *const argv[]){
    int i;
    for (i = 0; i < slaveNum; i++){
        int fdFatherToSon[2]; //father(1) ---> son(0)
        int fdSonToFather[2]; // son(1) ---> father(0)

        checkError(pipe(fdFatherToSon),"pipe_fdFatherToSon");
        checkError(pipe(fdSonToFather),"pipe_fdSonToFather");

        pid_t pid;
        checkError((pid=fork()),"fork");

        if (pid == 0){  //hijo
            close(fdFatherToSon[WRITE]); //cierro los fd correspondientes al padre
            close(fdSonToFather[READ]);
            close(0);
            close(1);

            checkError(dup2(fdFatherToSon[READ], 0),"dup2_fdFatherToSon");
            checkError(dup2(fdSonToFather[WRITE], 1),"dup2_fdSonToFather");

            close(fdFatherToSon[READ]);
            close(fdSonToFather[WRITE]);

            execv(path, argv);
            exit(-1);
        }
        else{   //padre
            close(fdSonToFather[WRITE]); //cierro los fd correspondientes al hijo
            close(fdFatherToSon[READ]);

            fd[i][READ] = fdSonToFather[READ];
            fd[i][WRITE] = fdFatherToSon[WRITE];
        }
    }
}
