#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

int main(int argc, char *argv[])
{
    char *grepArgs[] = {"/usr/bin/grep", "system", NULL};
    char *environ[] = {NULL};
    char *grepArgsFinal[100];
    int index = 0;
    int flag = 0;

    for (int i = 0; i < argc; i++)
    {
        char *token = strtok(argv[i], "[\"\']");
        
        while (token != NULL && token != "")
        {
            if ((strcmp(token, "<") == 0) || (strcmp(token, ">") == 0) || (strcmp(token, ">>") == 0))
            {
                flag = 1;
                break;
            }
            grepArgsFinal[index++] = token;
            token = strtok(NULL, "[\"\']");
        }

        if (flag)
        {
            break;
        }
    }

    grepArgsFinal[index] = NULL;
    
    execve("/usr/bin/grep", grepArgsFinal, environ);
    
    exit(0);
}