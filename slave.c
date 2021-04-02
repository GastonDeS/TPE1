#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <errno.h>
#include <stdlib.h>

int checkCNF(const char *path);

int main(int argc, char const *argv[]) {

    char const minisat[] = {"minisat %s | grep -o -e \"Number of.*[0-9]\\+\" -e \"CPU time.*\" -e \".*SATISFIABLE\""};
    int minisat_size = (sizeof(minisat)/sizeof(minisat[0]) -2); //la dimencion de minisat sin %s
    char *fileName = NULL;
    size_t fileNameSize = 0;
    size_t numChar;

    while ((numChar = getdelim(&fileName, &fileNameSize, '\n',stdin)) > 0){ 
        
        //preparacion de parametros para minisat
        char command[minisat_size+numChar-1];
        fileName[numChar-1] = 0; //le saco el salto de linea
        checkCNF(fileName);//si no existe el archivo o no se permite la lectura termina la ejecucion 
        sprintf(command, minisat, fileName);
        char *const params[] = {command, NULL};
    
        //minisat
        FILE * fp;
        char *minisatReturn = NULL;
        size_t minisatReturnSize = 0;
        size_t minisatReturnDim;
        if (( fp = popen(*params, "r")) == NULL ){
            perror("popen");
            exit(-1);
        }
        minisatReturnDim = getdelim(&(minisatReturn), &minisatReturnSize,'\0', fp);

        //get pid
        char pid[10]; // Mejorar!!!
        int pidDim ;
        pidDim = sprintf(pid, "%d\n", getpid());

        //imprimir resultado final
        fileName[numChar-1] = '\n'; //le vuelvo agregar el salo de linea
        strcat(fileName, minisatReturn);
        strcat(fileName,pid);
        write(1, fileName, (minisatReturnDim+numChar+pidDim));

        pclose(fp);
        free(minisatReturn);
    }

    if(numChar == -1 && (errno == EINVAL || errno == ENOMEM)){
        perror("getline");
        exit(-1);
    }

    free(fileName);
    return 0;
}

int checkCNF(const char *path){
    if(access(path, R_OK) == 0)
        return 0;
    perror("invalid path");
    exit(-1);
}