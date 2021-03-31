#include <stdio.h>
#include <unistd.h>
#include <string.h>



int main(int argc, char const *argv[]) { 

    char const minisat[] = {"minisat %s | grep -o -e \"Number of.*[0-9]\\+\" -e \"CPU time.*\" -e \".*SATISFIABLE\""};
    int minisat_size = (sizeof(minisat)/sizeof(minisat[0]) -2); //le saco %s
    
    char *buff = NULL;
    size_t buffSize = 0;
    size_t result;

    while ((result = getline(&buff, &buffSize, stdin)) > 0){
        
        
        char command[minisat_size+result-1];
        buff[result-1] = 0;
        

        sprintf(command, minisat, buff);
        //printf("%s",command);
        

        
        
        //char *const params[] = {command, NULL};
        char *const params[] = {"minisat ./files/pigeon-hole/hole6.cnf |  grep -o -e \"Number of.*[0-9]\\+\" -e \"CPU time.*\" -e \".*SATISFIABLE\"", NULL};
        FILE * fp = popen(params, "w");
        sleep(1);
        pclose(fp);
        

    }
 
    return 0;
}
