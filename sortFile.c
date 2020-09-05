#include <fcntl.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    int filesFD = open(argv[1], O_RDONLY);
    char *errorMessage = "Illegal command or arguments\n";
    char *sortArgs[] = {"/usr/bin/sort", NULL};
    char *environ[] = {NULL};

    if (filesFD == -1)
    {
        write(STDERR_FILENO, errorMessage, strlen(errorMessage));
        exit(0);
    }

    dup2(filesFD, STDIN_FILENO);
    close(filesFD);
    execve("/usr/bin/sort", sortArgs, environ);    

    return 0;
}