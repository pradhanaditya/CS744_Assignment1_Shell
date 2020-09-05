#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char *environ[] = {NULL};
    int index = 0;
    int isIllegalCommand = 0;
    int redirectionFlag = 0;
    char *errorMessage = "Illegal command or arguments\n";
    int isCommandValid;
    char *catArgsFinal[100];

    for (int i = 0; i < argc; i++)
    {
        if ((strcmp(argv[i], ">") == 0) || (strcmp(argv[i], ">>") == 0))
        {
            redirectionFlag = 1;
            break;        
        }
        else if (strcmp(argv[i], "<") == 0)
        {
            isIllegalCommand = 1;
            break;
        }
        else if (strcmp(argv[i], "|") == 0)
        {
            break;
        }
        else
        {
            catArgsFinal[index++] = argv[i];
        }
    }

    catArgsFinal[index] = NULL;

    if (isIllegalCommand)
    {
        write(STDERR_FILENO, errorMessage, strlen(errorMessage));
        exit(0);
    }

    if (redirectionFlag)
    {
        isCommandValid = execve("/usr/bin/cat", catArgsFinal, environ);
    }
    else
    {
        isCommandValid = execve("/usr/bin/cat", catArgsFinal, environ);
    }
    
    
    if (isCommandValid == -1)
    {
        write(STDERR_FILENO, errorMessage, strlen(errorMessage));
        return 0;
    }

    exit(0);
}