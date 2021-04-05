#include "slave.h"

int main(int argc, char const *argv[]) {

    int minisat_size = (sizeof(MINITA)/sizeof(MINITA[0]) -2); //la dimencion de minisat sin %s
    char *fileName = NULL;
    size_t fileNameSize = 0;
    ssize_t fileNameDim;

    while ((fileNameDim = getline(&fileName, &fileNameSize,stdin)) > 0){
        char command[minisat_size+fileNameDim-1];
        fileName[fileNameDim-1] = 0; //le saco el salto de linea
        checkFile(fileName);//si no existe el archivo o no se permite la lectura termina la ejecucion 
        sprintf(command, MINITA, fileName);
        char *const params[] = {command, NULL};
    
        char *minisatReturn = NULL;
        size_t minisatReturnSize = 0;
        size_t minisatReturnDim;
        FILE * fp;
        checkErrno( ( fp=popen(*params, "r")), "popen",NULL);
        minisatReturnDim = getdelim(&minisatReturn, &minisatReturnSize,'\0', fp); 
        if(minisatReturnDim == -1 && (errno == EINVAL || errno == ENOMEM)){
            perror("getline");
            exit(EXIT_FAILURE);
        }

        //get pid
        char pid[10];
        int pidDim ;
        pidDim = sprintf(pid, "%d\n", getpid());

        //imprimir resultado final
        fileName[fileNameDim-1] = '\n'; //le vuelvo agregar el salto de linea       
        int resultDim = minisatReturnDim+fileNameDim+pidDim;
        char result[resultDim];
        int i;
        for (i = 0; i < resultDim; i++){
            if(i < fileNameDim)
                result[i] = fileName[i];
            else if(i < (fileNameDim+minisatReturnDim) )
                result[i] = minisatReturn[i-fileNameDim];
            else if(i < (fileNameDim+minisatReturnDim+pidDim))
                result[i] = pid[i-(fileNameDim+minisatReturnDim)];
        }
        write(1, result, resultDim);
        free(minisatReturn);
    }

    if(fileNameDim == -1 && (errno == EINVAL || errno == ENOMEM)){
        perror("getline");
        exit(-1);
    }

    free(fileName);
    return 0;
}

int checkFile(const char *path){
    if(access(path, R_OK) == 0)
        return 0;
    perror("invalid path");
    exit(-1);
}

