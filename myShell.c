#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <stdlib.h>
#include <signal.h>

void sigint_handler() 
{
    char *confMessage = "the program is interrupted, do you want to exit [Y/N]";
    char *c = (char *) malloc(2*sizeof(char));
    write(STDOUT_FILENO, confMessage, strlen(confMessage));
    read(STDIN_FILENO, c, 2);
    c[1] = '\0';

    if (strcmp(c, "Y") == 0)
    {
        exit(0);
    }

    char *shellPrompt = "myShell> ";
    write(STDOUT_FILENO, (void *) shellPrompt, strlen(shellPrompt));
    free(c);
}

void sigterm_handler()
{
    char *message = "Got SIGTERM-Leaving";
    write(STDOUT_FILENO, message, strlen(message));
    exit(0);
}

int main()
{
    char *newLine = "\n";
    char *emptyChar = "";
    char *inputCommand;
    signal(SIGINT, sigint_handler);
    signal(SIGTERM, sigterm_handler);

    while (1)
    {
        fflush(stdin);
        fflush(stdout);
        fflush(stderr);
        int fd1[2];
        int fd2[2];
        pipe(fd1);
        pipe(fd2);
        char *shellPrompt = "myShell> ";
        inputCommand = (char *) calloc(1000, sizeof(char));
        int index = 0;
        char tempChar;
        char *commandArgs[2][1000];
        int hasPipe[2] = {0};
        char *pipeCommandArgs[2][1000];
        char *environ[] = {NULL};
        char *errorMessage = "Illegal command or arguments\n";
        char *commands[10];
        int commandsCounter = 0;
        int wstatus1, wstatus2, wstatus3, wstatus4;

        for (int i = 0; i < 2; i++)
        {
            for (int j = 0; j < 1000; j++)
            {
                commandArgs[i][j] = '\0';
                pipeCommandArgs[i][j] = '\0';
            }
        }

        for (int i = 0; i < 10; i++)
        {
            commands[i] = '\0';
        }

        write(STDOUT_FILENO, (void *) shellPrompt, strlen(shellPrompt));
        
        read(STDIN_FILENO, inputCommand, 1000);
        
        char *commandSeparation = strtok(inputCommand, ";");

        while (commandSeparation != NULL)
        {
            commands[commandsCounter++] = commandSeparation;
            commandSeparation = strtok(NULL, ";");
        }

        int childProcessIDs[commandsCounter];
        int pipeProcessIDs[2] = {-1, -1};

        for (int i = 0; i < commandsCounter; i++)
        {
            char *token = strtok(commands[i], "[\n ][\n ]*");
            int argsIndex = 0;
            int pipeArgsIndex = 0;
            while (token != NULL)
            {
                commandArgs[i][argsIndex++] = token;
                token = strtok(NULL, "[\n ][\n ]*");
            }

            if ((strcmp(commandArgs[i][0], "exit") == 0) && argsIndex == 1)
            {
               return 0;
            }

            for (int a = 0; a < argsIndex; a++)
            {
                if (strcmp(commandArgs[i][a], "|") == 0)
                {
                    hasPipe[i] = 1;
                    for (int k = a + 1; k < argsIndex; k++)
                    {
                        pipeCommandArgs[i][pipeArgsIndex++] = commandArgs[i][k]; 
                    }
                    break;
                }
            }

            childProcessIDs[i] = fork();

            if (childProcessIDs[i] == 0)
            {
                for (int j = 1; j < argsIndex - 1; j++)
                {
                    if (strcmp(commandArgs[i][j], "<") == 0)
                    {
                        int inputFD = open(commandArgs[i][j + 1], O_RDONLY);
                        if (inputFD == -1)
                        {
                            write(STDERR_FILENO, errorMessage, strlen(errorMessage));
                            return 0;
                        }
                        
                        dup2(inputFD, STDIN_FILENO);
                        close(inputFD);
                    }
                    else if (strcmp(commandArgs[i][j], ">") == 0)
                    {
                        int outputFD = open(commandArgs[i][j + 1], O_CREAT | O_TRUNC | O_RDWR);
                        dup2(outputFD, STDOUT_FILENO);
                        close(outputFD);
                    }
                    else if (strcmp(commandArgs[i][j], ">>") == 0)
                    {
                        int appendFD = open(commandArgs[i][j + 1], O_CREAT | O_WRONLY | O_APPEND);
                        dup2(appendFD, STDOUT_FILENO);
                        close(appendFD);
                    }
                    else if (strcmp(commandArgs[i][j], "|") == 0)
                    {
                        hasPipe[i] = 1;

                        if (i == 0)
                        {
                            dup2(fd1[1], STDOUT_FILENO);
                            close(fd1[0]);
                            close(fd1[1]);
                        }
                        else
                        {
                            dup2(fd2[1], STDOUT_FILENO);
                            close(fd2[1]);
                            close(fd2[0]);
                        }

                        break;
                    }
                }

                int isCommandValid = execve(commandArgs[i][0], commandArgs[i], environ);

                if (isCommandValid == -1)
                {
                    write(STDERR_FILENO, errorMessage, strlen(errorMessage));
                    return 0;
                }

                break;
            }
            else
            {
                if (hasPipe[i])
                {
                    if (i == 0)
                    {
                        waitpid(childProcessIDs[0], &wstatus1, 0);
                    }
                    else
                    {
                        waitpid(childProcessIDs[1], &wstatus2, 0);
                    }

                    pipeProcessIDs[i] = fork();

                    if (pipeProcessIDs[i] == 0)
                    {
                        if (i == 0)
                        {
                            dup2(fd1[0], STDIN_FILENO);
                            close(fd1[1]);
                            close(fd1[0]);
                        }
                        else
                        {
                            dup2(fd2[0], STDIN_FILENO);
                            close(fd2[1]);
                            close(fd2[0]);
                        }
                      
                        for (int j = 1; j < pipeArgsIndex - 1; j++)
                        {
                            if (strcmp(pipeCommandArgs[i][j], ">") == 0)
                            {
                                int outputFD = open(pipeCommandArgs[i][j + 1], O_CREAT | O_TRUNC | O_RDWR);
                                dup2(outputFD, STDOUT_FILENO);
                                close(outputFD);
                            }
                            else if (strcmp(pipeCommandArgs[i][j], ">>") == 0)
                            {
                                int appendFD = open(pipeCommandArgs[i][j + 1], O_CREAT | O_WRONLY | O_APPEND);
                                dup2(appendFD, STDOUT_FILENO);
                                close(appendFD);
                            }
                        }

                        int isCommandValid = execve(pipeCommandArgs[i][0], pipeCommandArgs[i], environ);

                        if (isCommandValid == -1)
                        {
                            write(STDERR_FILENO, errorMessage, strlen(errorMessage));
                            return 0;
                        }
                    }
                    else
                    {
                        if (i == 0)
                        {
                            close(fd1[0]);
                            close(fd1[1]);    
                        }
                        else
                        {
                            close(fd2[0]);
                            close(fd2[1]);
                        }
                    }
                    
                }
            }
        }

        if (commandsCounter == 1)
        {
            if (!hasPipe[0])
            {
                waitpid(childProcessIDs[0], &wstatus1, 0);
            }
            if (hasPipe[0])
            {
                waitpid(pipeProcessIDs[0], &wstatus3, 0);
            }
        }
        else
        {
            if (!hasPipe[0])
            {
                waitpid(childProcessIDs[0], &wstatus1, 0);
            }
            if (hasPipe[0])
            {
                waitpid(pipeProcessIDs[0], &wstatus3, 0);
            }
            if (!hasPipe[1])
            {
                waitpid(childProcessIDs[1], &wstatus2, 0);
            }
            if (hasPipe[1])
            {
                waitpid(pipeProcessIDs[1], &wstatus4, 0);
            }
        }

        free(inputCommand);
    }

    //Printing the newline character to STDOUT
    write(STDOUT_FILENO, (void *) newLine, strlen(newLine));

    return 0;
}