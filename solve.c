#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>

#define NUM_CHILD 8
#define READ 0
#define WRITE 1
#define PATHS_INI 8

int main(int argc, char **argv){

    //int child = (NUM_CHILD<argc)?NUM_CHILD:argc;
    //child=4;
    char *const *argvs = NULL;
    int fd[NUM_CHILD][2];
    //int pidC[NUM_CHILD];

    int i;
    for (i = 0; i < NUM_CHILD; i++){
        int fdFatherToSon[2]; //father(1) ---> son(0)
        int fdSonToFather[2]; // son(1) ---> father(0)
        if (pipe(fdFatherToSon) < 0 || pipe(fdSonToFather) < 0){ 
            perror("pipe");
            exit(-1);
        }

        pid_t pid = fork();
        if (pid < 0){
            perror("fork");
            exit(-1);
        }
        if (pid == 0){  //hijo
            close(fdFatherToSon[WRITE]); //cierro los fd correspondientes al padre
            close(fdSonToFather[READ]);
            close(0);
            close(1);
            if (dup2(fdFatherToSon[READ], 0) < 0 || dup2(fdSonToFather[WRITE], 1) < 0)
            { // la entrada de escritura del pipe quedo conetada con el stdout
                perror("dup2");
                exit(-1);
            }
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
    int argCount;
    for (argCount = 0; argCount < PATHS_INI; argCount++){
        char path[4096]={0};
        strcat(path,argv[argCount+1]);
        strcat(path,"\n");
        if (write(fd[argCount%NUM_CHILD][WRITE],path ,strlen(path)) < 0)
            perror("write");
    }
    int goodCount =1;
    //loop
    while (goodCount < argc){ // mientras haya cnf para analizar
        fd_set readfds;
        FD_ZERO(&readfds);
        int i,max=0;
        for (i = 0; i < NUM_CHILD; i++){
            if (fd[i][READ] > 0) {
                max = (fd[i][READ]>max)?fd[i][READ] : max; 
                FD_SET(fd[i][READ], &readfds);
            }
        }
        int ready = select(max+1, &readfds, NULL, NULL, NULL);

        if (ready == -1) {  //compruebo errores del select
            perror("select()");
            exit(EXIT_FAILURE);
        }
        
        for (i = 0; i < NUM_CHILD && ready > 0; i++){ 
            if(FD_ISSET(fd[i][READ], &readfds) != 0){
                goodCount++;
                //prueba de funcionamiento--------------------------------
                char buff[512]={0};
                read(fd[i][READ], buff, sizeof(buff));
                printf("%s \n", buff);
                //----------------------------------------------------------
                //envio tasks
                if (argCount < argc-1) {
                    char path[4096]={0};
                    char *vec;
                    vec = strcat(path,argv[argCount+1]);
                    vec = strcat(vec,"\n");
                    if (write(fd[i][WRITE],vec ,strlen(vec)) < 0)
                        perror("write");
                    argCount++;
                }
                //----------------------------------------------------------
                //lee el resultado 
                //le mando el proximo archivo, sino cerrar el pipe
                ready--;  
            }
        }   
    }
    return 0;
}