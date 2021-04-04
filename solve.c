#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>
#include <sys/shm.h>
#include <fcntl.h>
#include <semaphore.h>
#include <sys/mman.h>

#define NUM_CHILD 8
#define READ 0
#define WRITE 1
#define PATHS_INI 16
#define SHM_NAME "/sharedMemory"

//#define SHM_STEP 200

#define SIZE_SHM 512


void* initShM(char* const name, int* fdShm, off_t* sizeShm);
void checkError(int valueReturn, const char *errorMessage);
void checkErrno(void* valueReturn, const char *errorMessage, void* numErrno);

int main(int argc, char const *argv[]){

    if (argc == 1) {
        printf("Error en la cantiad de argumentos/n");
        return 1;
    }

    //int child = (NUM_CHILD<argc)?NUM_CHILD:argc;
    //child=4;
    char *const *argvs = NULL;
    int fd[NUM_CHILD][2];

    //SHM
    //off_t sizeShm = SHM_STEP*(argc -1); 
    off_t sizeShm = SHM_STEP;
    int fdShm;
    void * pntShm;

    //Sem
    sem_t *semShm;


    FILE * result = fopen("result.txt", "w");

    if(setvbuf(stdout, NULL, _IONBF, 0))//revisar
        perror("Error setvbuf");

    pntShm = initShM(SHM_NAME, &fdShm, &sizeShm);
    
//    checkError((fdShm=shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666)), "shmopen");
//    checkError(ftruncate(fdShm, sizeShm),"ftrunate" );
//    checkErrno((pntShm=mmap(NULL, sizeShm, PROT_READ | PROT_WRITE, MAP_SHARED, fdShm, 0)), "SHM_map", MAP_FAILED);

    checkErrno((semShm=sem_open("semShm", O_CREAT, 0700, 0)), "sem_open", SEM_FAILED);

    printf("%s %d\n", SHM_NAME, (int)sizeShm); //impreimo datos necesarios para view
    sleep(2);


    int i;
    for (i = 0; i < NUM_CHILD; i++){
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

            execv("./slave", argvs); 
            perror("execv");
            exit(-1);
        }
        else{   //padre
            close(fdSonToFather[WRITE]); //cierro los fd correspondientes al hijo
            close(fdFatherToSon[READ]);

            fd[i][READ] = fdSonToFather[READ];
            fd[i][WRITE] = fdFatherToSon[WRITE];

        }
    }

    //asignacion de la primera tanda de archivos
    int fileSentCount = 0;
    for (i = 0; i < PATHS_INI; i++){
        char path[4096]={0};
        strcat(path,argv[i+1]);
        strcat(path,"\n");

        checkError(write(fd[i%NUM_CHILD][WRITE],path ,strlen(path)),"write");
        fileSentCount++;
    }

    int fileRecivedCount = 0;
    char* shIndex = (char *) pntShm;

    //loop
    while (fileRecivedCount < (argc-1)){ // mientras haya cnf para analizar
        fd_set readfds;
        FD_ZERO(&readfds);
        int i,max=0;
        for (i = 0; i < NUM_CHILD; i++){
            if (fd[i][READ] > 0) {
                max = (fd[i][READ]>max)?fd[i][READ] : max; 
                FD_SET(fd[i][READ], &readfds);
            }
        }
        int ready=0;
        checkError((ready=select(max+1, &readfds, NULL, NULL, NULL)),"select");
        
        for (i = 0; i < NUM_CHILD && ready > 0; i++){ 
            if(FD_ISSET(fd[i][READ], &readfds) != 0){
                
                //recibo un file
                char buff[512]={0};
                if(read(fd[i][READ], buff, sizeof(buff)) <= 0){
                    fd[i][READ] = -1;//el hijo termino
                    continue;
                }
                fileRecivedCount++;

                if(fileRecivedCount > (argc-(PATHS_INI-NUM_CHILD)+1)){  //voy cerrando los hijos que le queda un solo 
                    fd[i][READ] = -1;
                }

                //envio respuesta el viewer
                checkError(sprintf((char *)(shIndex),"%s \n", buff),"sprint");

                checkError(sem_post(semShm),"post sem"); //revisar

                //shIndex += strlen(buff) + 1;
                shIndex += SHM_STEP;
                

                //envio tasks
                if (fileSentCount < argc-1) {
                    char path[4096]={0};
                    char *vec;
                    vec = strcat(path,argv[fileSentCount+1]);
                    vec = strcat(vec,"\n");

                    checkError(write(fd[i][WRITE],vec ,strlen(vec)),"wirte_sendTasks");
                    fileSentCount++;
                }
                ready--; 
            }
        }
    }
    close(fd[i][READ]);
    close(fd[i][WRITE]);
    
    return 0;

}


void checkError(int valueReturn, const char *errorMessage){
    if(valueReturn < 0){
        perror(errorMessage);
        exit(-1);
    }
}

void checkErrno(void* valueReturn, const char *errorMessage, void* numErrno){
    if(valueReturn == numErrno){
        perror(errorMessage);
        exit(EXIT_FAILURE);
    }
}

void* initShM(char* const name, int* fdShm, off_t* sizeShm){
    void* pntShm;
    checkError( ((*fdShm)=shm_open(name, O_CREAT | O_RDWR, 0666)) , "shmopen");
    checkError(ftruncate(*fdShm, *sizeShm),"ftrunate" );
    checkErrno( (pntShm=mmap(NULL, *sizeShm, PROT_READ | PROT_WRITE, MAP_SHARED, *fdShm, 0)) , "SHM_map", MAP_FAILED);
    return pntShm;
}