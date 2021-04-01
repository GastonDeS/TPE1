#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CHILD 8

void readAndPrint(int fd);

int main(int argc, char const *argv[]) {

    int child = (CHILD<argc)?CHILD:argc;
    child=1;
    int pidC[child]; //pid
    int fdFtoS[child][2];
    int fdStoF[child][2];
    int ppid = getpid();
    char *const *argvs = NULL;
    int i;
    for ( i = 0; i < child; i++) {
        if (pipe(fdFtoS[i]) ==-1) {
            perror("pipe");
            exit(-1);
        }
        if (pipe(fdStoF[i]) ==-1) {
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
            dup2(fdFtoS[i][0],0);
            dup2(fdStoF[i][1],1);
            close(fdStoF[i][0]);
            close(fdStoF[i][1]);
            close(fdFtoS[i][0]);
            close(fdFtoS[i][1]);
            execv("./slave",argvs); //crear esclavo
            perror("execv");
            exit(-1);
        } //padre
        close(fdFtoS[i][0]);
    }
    for ( i = 0; i < child; i++) {
        if( write(fdFtoS[i][1],"files/pigeon-hole/hole6.cnf\n",28) == -1) {
            perror("write");
            exit(-1);
        }
        close(fdFtoS[i][1]);
    }
    sleep(1);
    readAndPrint(fdStoF[0][0]);

    return 0;
}

void readAndPrint(int fd){
    char line[100];
    size_t linecap = 100;
    ssize_t linelen;
    int linecount = 0;
    while ((linelen= read(fd,line,linecap)) > 0) {
        linecount++;
        fwrite(line, linelen, 1, stdout);
    }
}
