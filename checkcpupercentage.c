#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

extern int charToInteger(char c);

enum fileDescriptors {STDIN = 0, STDOUT = 1, STDERR = 2};

int main(int argc, char *argv[])
{
    char *errorMessage = "Illegal command or arguments";
    char *newLine = "\n";
    char *statBuffer = (char *) calloc(1000, sizeof(char));
    //char statBuffer[1000];
    char *statCPUBuffer = (char *) calloc(1000, sizeof(char));

    /* Checking for a pipe and performing I/O redirection accordingly 
    for (int i = 1; i < argc; i++)
    {
        if (strcmp(argv[i], "|") == 0)
        {

        }
    }   */

   /* if (argc > 2)
    {
        write(STDERR, (void *) errorMessage, strlen(errorMessage));
        return 1;
    } */

    char procDirectory1[50] = "/proc";
    char procDirectory2[50] = "/proc";
    char processName[50] = "/";
    strcat(processName, argv[1]);
    strcat(procDirectory1, processName);
    strcat(procDirectory1, "/stat");
    int processStatFD = open(procDirectory1, O_RDONLY);

    read(processStatFD, statBuffer, 1000);

    strcat(procDirectory2, "/stat");
    
    int cpuStatFD = open(procDirectory2, O_RDONLY);
    
    read(cpuStatFD, statCPUBuffer, 100);

    int statBufferIndex = 0;
    int cpuStatIndex = 0;

    for (int i = 0; i < 13; i++)
    {
        while (statBuffer[statBufferIndex] != ' ')
        {
            statBufferIndex++;
        }

        statBufferIndex++;
    }

    long long int userTime1 = 0;
    long long int sysTime1 = 0;
    long long int cpuTime1 = 0;
    long long int tempCpuTime = 0;

    while (statBuffer[statBufferIndex] != ' ')
    {
        userTime1 = userTime1 * 10 + charToInteger(statBuffer[statBufferIndex]);
        statBufferIndex++;
    }

    statBufferIndex++;

    while (statBuffer[statBufferIndex] != ' ')
    {
        sysTime1 = sysTime1 * 10 + charToInteger(statBuffer[statBufferIndex]);
        statBufferIndex++;
    }

    for (int i = 5; statCPUBuffer[i] != '\n'; i++) //Skipping the 'cpu  ' part
    {
        int tempInt = charToInteger(statCPUBuffer[i]);
        
        if (tempInt == -1)
        {
            cpuTime1 = cpuTime1 + tempCpuTime;
            tempCpuTime = 0;
        }
        else
        {
            tempCpuTime = tempCpuTime * 10 + tempInt;   
        }
    }
    cpuTime1 = cpuTime1 + tempCpuTime;

    tempCpuTime = 0;

    close(processStatFD);
    close(cpuStatFD);
    free(statBuffer);
    free(statCPUBuffer);

    sleep(1);

    long long int userTime2 = 0;
    long long int sysTime2 = 0;
    long long int cpuTime2 = 0;
    processStatFD = open(procDirectory1, O_RDONLY);
    cpuStatFD = open(procDirectory2, O_RDONLY);
    statBuffer = (char *) calloc(1000, sizeof(char));
    statCPUBuffer = (char *) calloc(1000, sizeof(char));
    read(processStatFD, statBuffer, 1000);
    read(cpuStatFD, statCPUBuffer, 100);

    statBufferIndex = 0;

    for (int i = 0; i < 13; i++)
    {
        while (statBuffer[statBufferIndex] != ' ')
        {
            statBufferIndex++;
        }

        statBufferIndex++;
    }

    while (statBuffer[statBufferIndex] != ' ')
    {
        userTime2 = userTime2 * 10 + charToInteger(statBuffer[statBufferIndex]);
        statBufferIndex++;
    }

    statBufferIndex++;

    while (statBuffer[statBufferIndex] != ' ')
    {
        sysTime2 = sysTime2 * 10 + charToInteger(statBuffer[statBufferIndex]);
        statBufferIndex++;
    }

    for (int i = 5; statCPUBuffer[i] != '\n'; i++) //Skipping the 'cpu  ' part
    {
        int tempInt = charToInteger(statCPUBuffer[i]);
        
        if (tempInt == -1)
        {
            cpuTime2 = cpuTime2 + tempCpuTime;
            tempCpuTime = 0;
        }
        else
        {
            tempCpuTime = tempCpuTime * 10 + tempInt;   
        }
    }

    cpuTime2 = cpuTime2 + tempCpuTime;

    long long int numberOfProcessors = sysconf(_SC_NPROCESSORS_ONLN);

    float userResult = (numberOfProcessors * (userTime2 - userTime1) * 100.0)/(cpuTime2 - cpuTime1);
    float sysResult = (numberOfProcessors * (sysTime2 - sysTime1) * 100.0)/(cpuTime2 - cpuTime1);
    
    char *userModeMessage = "user mode cpu percentage: ";
    char *sysModeMessage = "system mode cpu percentage: ";

    char *buf1 = (char *) calloc(10, sizeof(char));
    char *buf2 = (char *) calloc(10, sizeof(char));
    gcvt(userResult, 6, buf1);
    gcvt(sysResult, 6, buf2);
    strcat(buf1, "%");
    strcat(buf2, "%");
    write(STDOUT, userModeMessage, strlen(userModeMessage));
    write(STDOUT, buf1, strlen(buf1));
    write(STDOUT, newLine, strlen(newLine));
    write(STDOUT, sysModeMessage, strlen(sysModeMessage));
    write(STDOUT, buf2, strlen(buf2));
    write(STDOUT, newLine, strlen(newLine));
    
    exit(0);
}