#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>
#include <fcntl.h>

int main(int argc, char *argv[])
{
    char *fileContent = (void *) malloc(10000*sizeof(char));
    int fd = open(argv[1], O_RDONLY);
    read(fd, fileContent, 10000);
    char *command = strtok_r(fileContent, "[\n;]", &fileContent);
    char *errorMessage = "Illegal command or arguments\n";
    char *environ[] = {NULL};
    
    while (command != NULL)
    {
        char *commandArgs[20];
        int argIndex = 0;
        
        char *argToken = strtok(command, " ");

        while (argToken != NULL)
        {
            commandArgs[argIndex++] = argToken;
            argToken = strtok(NULL, " ");
        }

        int childPID = fork();

        if (childPID == 0)
        {
            int isCommandValid = execve(commandArgs[0], commandArgs, environ);

            if (isCommandValid == -1)
            {
                write(STDOUT_FILENO, errorMessage, strlen(errorMessage));
                return 0;
            }
        }
        else
        {
            wait(NULL);
        }
        
        command = strtok_r(fileContent, "[\n;]", &fileContent);
    }

    return 0;
}