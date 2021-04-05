 #ifndef ERR_H
 #define ERR_H

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>

void checkError(int valueReturn, const char *errorMessage);
void checkErrno(void* valueReturn, const char *errorMessage, void* numErrno);

#endif 