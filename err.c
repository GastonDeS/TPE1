#include "err.h"

void checkError(int valueReturn, const char *errorMessage){
    if(valueReturn == -1){
        perror(errorMessage);
        exit(EXIT_FAILURE);
    }
}

void checkErrno(void* valueReturn, const char *errorMessage, void* numErrno){
    if(valueReturn == numErrno){
        perror(errorMessage);
        exit(EXIT_FAILURE);
    }
}
