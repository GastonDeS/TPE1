 #ifndef SOLVE_H
 #define SOLVE_H

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
#include "err.h"

#define NUM_CHILD 8
#define READ 0
#define WRITE 1
#define SLAVE_PATH "./slave"
#define PATHS_INI 16
#define RESULT_FILE_NAME "result.txt"
#define SHM_NAME "/sharedMemory"

#define STEP_SHM 200//sacar de aca

void initSlave(int slaveNum, int fd[][2], const char *path, char *const argv[]);
void* initShM(char* const name, int* fdShm, off_t* sizeShm);

#endif