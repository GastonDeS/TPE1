#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>

#define CHILD 8

int main(int argc, char const *argv[]) {
    int pidC[CHILD]; //pid
    int fd[CHILD][2];
    int ppid = getpid();
    char *const *argvs = NULL;
    int i;
    for ( i = 0; i < CHILD; i++) {
        if (pipe(fd[i]) ==-1) {
            perror("pipe");
            exit(-1);
        }
    }
    for ( i = 0; i < CHILD; i++){
        if (ppid == getpid()) pidC[i] = fork();
        if (pidC[i]==-1) {
            perror("fork");
            exit(-1);
        } else if (pidC[i] == 0){ //hijo
            // close(1);
            close(0);
            dup(fd[i][0]);
            // dup(fd[i][1]);
            close(fd[i][0]);
            close(fd[i][1]);
            execv("./c",argvs); //crear esclavo
            perror("execv");
            exit(-1);
        } //padre
        close(1);
        dup(fd[i][1]);
        close(fd[i][1]);
        close(fd[i][0]);
    	setvbuf(stdout, NULL, _IONBF, 0);
        write(fd[i][1],"hola",5);
    }
    
	
    return 0;
}
