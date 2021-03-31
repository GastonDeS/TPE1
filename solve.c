#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CHILD 8

int main(int argc, char const *argv[]) {

    int child = (CHILD<argc)?CHILD:argc;
    child=1;
    int pidC[child]; //pid
    int fd[child][2];
    int ppid = getpid();
    char *const *argvs = NULL;
    int i;
    for ( i = 0; i < child; i++) {
        if (pipe(fd[i]) ==-1) {
            perror("pipe");
            exit(-1);
        }
    }
    for ( i = 0; i < child; i++){
        if (ppid == getpid()) pidC[i] = fork();
        if (pidC[i]==-1) {
            perror("fork");
            exit(-1);
        } else if (pidC[i] == 0){ //hijo
            // close(1);
            close(0);
            dup(fd[i][0]);
            // dup(fd[i][1]);
            // close(fd[i][0]);
            close(fd[i][1]);
            execv("./slave",argvs); //crear esclavo
            perror("execv");
            exit(-1);
        } //padre
    }
    for ( i = 0; i < child; i++) {
        write(fd[i][0],"files/pigeon-hole/hole6.cnf",28);
    }
    
    sleep(1);

    return 0;
}
