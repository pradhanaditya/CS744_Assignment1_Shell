#include <fcntl.h>
#include <unistd.h>

int main()
{
    int filesFD = open("./files.txt", O_CREAT | O_WRONLY);
    char *lsArgs[] = {"ls", NULL};
    char *environ[] = {NULL};

    dup2(filesFD, STDOUT_FILENO);
    close(filesFD);
    execve("/usr/bin/ls", lsArgs, environ);    

    return 0;
}