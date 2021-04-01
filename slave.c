#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>

int checkCNF(char *path);

int main(int argc, char const *argv[]) {

    char const minisat[] = {"minisat %s | grep -o -e \"Number of.*[0-9]\\+\" -e \"CPU time.*\" -e \".*SATISFIABLE\""};
    int minisat_size = (sizeof(minisat)/sizeof(minisat[0]) -2); //calcula la dimencion del minsisat sin el %s
    char *buff = NULL;
    size_t buffSize = 0;
    int result;

    while ((result = getdelim(&buff, &buffSize, '\n',stdin)) > 0){ //esta bien que sea mayor que uno
        fwrite(buff,buffSize,1,stdout);
        putchar('\n');
        char command[minisat_size+result-1];
        printf("result: %d\n", result);
        buff[result-1] = 0;
        sprintf(command, minisat, buff);
        char *const params[] = {command, NULL};
        
        FILE * fp;
        if (( fp = popen(*params, "w")) == NULL ){
            perror("popen");
            exit(-1);
        }
        printf("%d\n",getpid());

        pclose(fp);
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
    valid = !S_ISDIR(buf.st_mode);
    return valid;
}
