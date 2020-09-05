#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>

int main(int argc, char *argv[])
{
    char *errorMessage = "Illegal command or arguments\n";
    char *fileContent = (void *) malloc(10000*sizeof(char));
    int fd = open(argv[1], O_RDONLY);

    if (fd == -1)
    {
        write(STDERR_FILENO, errorMessage, strlen(errorMessage));
        exit(0);
    }

    read(fd, fileContent, 10000);
    char *command = strtok_r(fileContent, "[\n;]", &fileContent);
    char *environ[] = {NULL};
    
    while (command != NULL)
    {
        int fd1[2];
        int fd2[2];
        pipe(fd1);
        pipe(fd2);
        char *commandArgs[20];
        char *pipeCommandArgs[1000];
        int hasPipe = 0;
        int argIndex = 0;
        int pipeArgsIndex = 0;
        int childProcessID;
        int wstatus1;
        int wstatus2;
        int pipeProcessID = -1;

        for (int i = 0; i < 20; i++)
        {
            commandArgs[i] = '\0';
        }

        for (int i = 0; i < 1000; i++)
        {
            pipeCommandArgs[i] = '\0';
        }

        char *argToken = strtok(command, "[\n ][\n ]*");

        while (argToken != NULL)
        {
            commandArgs[argIndex++] = argToken;
            argToken = strtok(NULL, "[\n ][\n ]*");
        }

        if ((strcmp(commandArgs[0], "exit") == 0) && argIndex == 1)
        {
           return 0;
        }

        for (int a = 0; a < argIndex; a++)
        {
            if (strcmp(commandArgs[a], "|") == 0)
            {
                hasPipe = 1;
                for (int k = a + 1; k < argIndex; k++)
                {
                    pipeCommandArgs[pipeArgsIndex++] = commandArgs[k]; 
                }
                break;
            }
        }

        childProcessID = fork();

        if (childProcessID == 0)
        {
            for (int j = 1; j < argIndex - 1; j++)
            {
                if (strcmp(commandArgs[j], "<") == 0)
                {
                    int inputFD = open(commandArgs[j + 1], O_RDONLY);
                    if (inputFD == -1)
                    {
                        write(STDERR_FILENO, errorMessage, strlen(errorMessage));
                        return 0;
                    }
                        
                    dup2(inputFD, STDIN_FILENO);
                    close(inputFD);
                }
                else if (strcmp(commandArgs[j], ">") == 0)
                {
                    int outputFD = open(commandArgs[j + 1], O_CREAT | O_TRUNC | O_RDWR);
                    dup2(outputFD, STDOUT_FILENO);
                    close(outputFD);
                }
                else if (strcmp(commandArgs[j], ">>") == 0)
                {
                    int appendFD = open(commandArgs[j + 1], O_CREAT | O_WRONLY | O_APPEND);
                    dup2(appendFD, STDOUT_FILENO);
                    close(appendFD);
                }
                else if (strcmp(commandArgs[j], "|") == 0)
                {
                    hasPipe = 1;

                    dup2(fd1[1], STDOUT_FILENO);
                    close(fd1[0]);
                    close(fd1[1]);
                    break;
                }
            }

            int isCommandValid = execve(commandArgs[0], commandArgs, environ);

            if (isCommandValid == -1)
            {
                write(STDERR_FILENO, errorMessage, strlen(errorMessage));
                return 0;
            }

            break;
        }
        else
        {
            if (hasPipe)
            {
                waitpid(childProcessID, &wstatus1, 0);
             
                pipeProcessID = fork();

                    if (pipeProcessID == 0)
                    {
                        dup2(fd1[0], STDIN_FILENO);
                        close(fd1[1]);
                        close(fd1[0]);
                        
                      
                        for (int j = 1; j < pipeArgsIndex - 1; j++)
                        {
                            if (strcmp(pipeCommandArgs[j], ">") == 0)
                            {
                                int outputFD = open(pipeCommandArgs[j + 1], O_CREAT | O_TRUNC | O_RDWR);
                                dup2(outputFD, STDOUT_FILENO);
                                close(outputFD);
                            }
                            else if (strcmp(pipeCommandArgs[j], ">>") == 0)
                            {
                                int appendFD = open(pipeCommandArgs[j + 1], O_CREAT | O_WRONLY | O_APPEND);
                                dup2(appendFD, STDOUT_FILENO);
                                close(appendFD);
                            }
                        }

                        int isCommandValid = execve(pipeCommandArgs[0], pipeCommandArgs, environ);

                        if (isCommandValid == -1)
                        {
                            write(STDERR_FILENO, errorMessage, strlen(errorMessage));
                            return 0;
                        }
                    }
                    else
                    {
                        close(fd1[0]);
                        close(fd1[1]);    
                    }
                }

                if (!hasPipe)
                {
                    waitpid(childProcessID, &wstatus1, 0);
                }
                if (hasPipe)
                {
                    waitpid(pipeProcessID, &wstatus2, 0);
                }
            }
        
        command = strtok_r(fileContent, "[\n;]", &fileContent);
    }

    return 0;
}