#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>

int main(int argc, char const *argv[]) { 

    char const minisat[] = {"minisat %s | grep -o -e \"Number of.*[0-9]\\+\" -e \"CPU time.*\" -e \".*SATISFIABLE\""};
    int minisat_size = (sizeof(minisat)/sizeof(minisat[0]) -2); //le saco %s
    
    char *buff = NULL;
    size_t buffSize = 0;
    size_t result;

    while ((result = getline(&buff, &buffSize, stdin)) > 1){ //esta bien que sea mayor que uno
        char command[minisat_size+result-1];
        buff[result-1] = 0;
        
        sprintf(command, minisat, buff);
        
        char *const params[] = {command, NULL};
        // char *const params[] = {"minisat ./files/pigeon-hole/hole7.cnf |  grep -o -e \"Number of.*[0-9]\\+\" -e \"CPU time.*\" -e \".*SATISFIABLE\"", NULL};
        FILE * fp = popen(*params, "w");
        pclose(fp);
        buff =NULL;
        buffSize =0;
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