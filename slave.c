#include <stdio.h>
#include <unistd.h>

int main(int argc, char const *argv[]) {
    
    char *const params[] = {"minisat ./files/pigeon-hole/hole6.cnf |  grep -o -e \"Number of.*[0-9]\\+\" -e \"CPU time.*\" -e \".*SATISFIABLE\"", NULL};
    FILE * fp = popen(*params, "w");
    
    pclose(fp);
    return 0;
}
