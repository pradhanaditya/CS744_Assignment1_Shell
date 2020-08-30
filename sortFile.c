#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>

int main(int argc, char *argv[])
{
    int filesFD = open(argv[1], O_RDONLY);
    char *sortArgs[] = {"/usr/bin/sort", NULL};
    char *environ[] = {NULL};

    dup2(filesFD, STDIN_FILENO);
    close(filesFD);
    execve("/usr/bin/sort", sortArgs, environ);    

    return 0;
}