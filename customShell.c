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
    else if (strcmp(c, "y") == 0)
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

int isACommandChar(char c)
{
    if (c >= 65 && c <= 122)
    {
        return 1;
    }
    
    return 0;
}

int main()
{
    char *newLine = "\n";
    char *emptyChar = "";
    signal(SIGINT, sigint_handler);
    signal(SIGTERM, sigterm_handler);

    while (1)
    {
        fflush(stdin);
        char *shellPrompt = "myShell> ";
        char *inputCommand = (char *) malloc(1000*sizeof(char));
        int index = 0;
        char tempChar;
        char *commandArgs[2][1000];
        char *environ[] = {NULL};
        char *errorMessage = "Illegal command or arguments\n";
        char *commands[10];
        int commandsCounter = 0;
        int wstatus1;
        int wstatus2;
        //Displaying the shell prompt to the user
        write(STDOUT_FILENO, (void *) shellPrompt, strlen(shellPrompt));

        read(STDIN_FILENO, inputCommand, 1000);

        char *commandSeparation = strtok(inputCommand, ";");

        while (commandSeparation != NULL)
        {
            commands[commandsCounter++] = commandSeparation;
            commandSeparation = strtok(NULL, ";");
        }

        int childProcessIDs[commandsCounter];

        for (int i = 0; i < commandsCounter; i++)
        {
            char *token = strtok(commands[i], "[\n ]");
            int argsIndex = 0;

            while (token != NULL)
            {
                commandArgs[i][argsIndex++] = token;
                token = strtok(NULL, "[\n ]");
            }

            if ((strcmp(commandArgs[i][0], "exit") == 0) && argsIndex == 1)
            {
               return 0;
            }

            childProcessIDs[i] = fork();

            if (childProcessIDs[i] == 0)
            {
                //Implementing the I/O redirection functionality here
                for (int j = 1; j < argsIndex - 1; j++)
                {
                    if (strcmp(commandArgs[i][j], "<") == 0)
                    {
                        int inputFD = open(commandArgs[i][j + 1], O_RDONLY);
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
                }
                break;
            }
        }

        if (childProcessIDs[0] == 0)
        {
            int isCommandValid = execve(commandArgs[0][0], commandArgs[0], environ);

            if (isCommandValid == -1)
            {
                write(STDOUT_FILENO, errorMessage, strlen(errorMessage));
                return 0;
            }
        }

        if (commandsCounter > 1 && childProcessIDs[1] == 0)
        {
            int isCommandValid = execve(commandArgs[1][0], commandArgs[1], environ);

            if (isCommandValid == -1)
            {
                write(STDOUT_FILENO, errorMessage, strlen(errorMessage));
                return 0;
            }
        }

        if (commandsCounter == 1)
        {
            waitpid(childProcessIDs[0], &wstatus1, 0);
        }
        else
        {
            waitpid(childProcessIDs[0], &wstatus1, 0);
            waitpid(childProcessIDs[1], &wstatus2, 0);
        }

        free(inputCommand);
    }

    //Printing the newline character to STDOUT
    write(STDOUT_FILENO, (void *) newLine, strlen(newLine));

    return 0;
}