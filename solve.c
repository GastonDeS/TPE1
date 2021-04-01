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
    child=2;
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
            dup2(fd[i][0],0);
            // dup2(fd[i][1],1);
            close(fd[i][0]);
            close(fd[i][1]);
            execv("./slave",argvs); //crear esclavo
            perror("execv");
            exit(-1);
        } //padre
        close(fd[i][0]);
    }
    for ( i = 0; i < child; i++) {
        if( write(fd[i][1],"files/pigeon-hole/hole6.cnf\n",29) == -1) {
            perror("write");
            exit(-1);
        }
        
    }
    sleep(1);
    for ( i = 0; i < child; i++) {
        if( write(fd[i][1],"files/pigeon-hole/hole6.cnf\n",29) == -1) {
            perror("write");
            exit(-1);
        }
        
    }
    exit(-1);
    return 0;
}
