#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/time.h>


#define CHILD 8

void readAndPrint(int fd);

int main(int argc, char const *argv[]) {

    int child = (CHILD<argc)?CHILD:argc;
    child=4;
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
            int j;
            for ( j = 0; j < child; j++) {
                close(fdStoF[j][0]);
                close(fdFtoS[j][1]);
            }
            for ( j = i; j < child; j++) { // en un for aparte ya que los fd anteriores a i ya fueron cerrados. evita errores en close.
                close(fdStoF[j][1]);
                close(fdFtoS[j][0]);
            }
            execv("./slave",argvs); //crear esclavo
            perror("execv");
            exit(-1);
        } //padre
        close(fdFtoS[i][0]);
        close(fdStoF[i][1]);
    }
    for ( i = 0; i < child; i++) {
        if( write(fdFtoS[i][1],"files/pigeon-hole/hole6.cnf\n",28) == -1) {
            perror("write");
            exit(-1);
        }
        close(fdFtoS[i][1]);
    }

    // select implementation
    int pipe;
    fd_set readfds;
    struct timeval timeout;
    //seteo los fd que se van a utilizar en el select
    FD_ZERO(&readfds);
    for ( i = 0; i < child; i++) {
        FD_SET(fdStoF[i][0],&readfds);
    }

    timeout.tv_sec = 5;
    timeout.tv_usec = 0;
    
    while ((pipe = select(fdStoF[child-1][0]+1,&readfds,NULL,NULL,&timeout)>0)){
        
        readAndPrint(fdStoF[pipe][0]);
        //asigno tareas
        // if( write(fdFtoS[i][1],"files/pigeon-hole/hole6.cnf\n",28) == -1) {
        //     perror("write");
        //     exit(-1);
        // }

        //re asigno a los fds para volver a select
        FD_ZERO(&readfds);
        for ( i = 0; i < child; i++) {
            FD_SET(fdStoF[i][0],&readfds);
        }
    }
    // fin de select

    return 0;
}

void readAndPrint(int fd){ // recibe el fd a leer
    char line[4096];
    size_t linecap = 4096;
    ssize_t linelen;
    int linecount = 0;
    while ((linelen= read(fd,line,linecap)) > 0) {
        linecount++;
        fwrite(line, linelen, 1, stdout);
    }
}
