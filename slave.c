#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char const *argv[]) { 

    char const minisat[] = {"minisat %s | grep -o -e \"Number of.*[0-9]\\+\" -e \"CPU time.*\" -e \".*SATISFIABLE\""};
    int minisat_size = (sizeof(minisat)/sizeof(minisat[0]) -2); //calcula la dimencion del minsisat sin el %s
    char *buff = NULL;
    int buffSize = 0;
    int result;

    while ((result = getline(&buff, &buffSize, stdin)) > 1){ 
        char command[minisat_size+result-1]; 
        buff[result-1] = 0;
        sprintf(command, minisat, buff);
        char *const params[] = {command, NULL};
        
        printf("%s",buff);
        FILE * fp = popen(*params, "w");
        printf("%s",getpid());

        pclose(fp);
        buff = NULL;
    }
    
    free(buff);
    
    if(result < 0){
        perror("getline");
        exit(-1);
    }

    return 0;
}

int checkCNF(char *path){
    struct stat buf;
    int valid=1;
    stat(path,&buf);
    valid = S_ISDIR(buf.st_mode);
    return valid;
}