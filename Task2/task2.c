#include <stdio.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>

#define INVALID_COUNT_ARGS "Еnter file to lock\n"
#define UNLOCK_ERROR "Unlock error\n"

pid_t PID;
int LOCKS = 0;

void killHandler(int sig) {
    int statLineLength = snprintf(NULL, 0, "Proccess with PID %d success locks: %d\n", PID, LOCKS);
    char* statLine = malloc(statLineLength + 1);
    snprintf(statLine, statLineLength+1, "Proccess with PID %d success locks: %d\n", PID, LOCKS);
    int fd = open("stat", O_WRONLY | O_APPEND | O_CREAT, 0664);
    write(fd, statLine, statLineLength);
    free(statLine);
    exit(0);
}

void lock(char* lockName, char* buffer, int bufferSize) {

    int FDLock = -1;
    while (FDLock == -1) {
        FDLock = open(lockName, O_CREAT | O_EXCL | O_WRONLY, 0664);
    }
    
    write(FDLock, buffer, bufferSize);
    close(FDLock);
}

void unlock(char* lockName, int bufferSize) {

    int FDLock = open(lockName, O_RDONLY, 0664);

    if (FDLock == -1) {
        printf(UNLOCK_ERROR);
        exit(1);
    }

    char buffer[bufferSize];

    int readResult = read(FDLock, buffer, bufferSize);
    if (readResult == -1) {
        printf(UNLOCK_ERROR);
        exit(1);
    }

    close(FDLock);

    int dataFromLck = atoi(buffer);

    if (dataFromLck != PID) {
        printf(UNLOCK_ERROR);
        exit(1);
    }

    remove(lockName);
}


int main(int argc, char* argv[]) {

    signal(SIGINT, killHandler);

    PID = getpid();

    if (argc < 2) {
        printf(INVALID_COUNT_ARGS);
        return 1;
    }

    char* fileName = argv[1];
    char* lockName = strcat(fileName, ".lck");

    char buffer[10];
    sprintf(buffer, "%d", PID);
    int bufferSize = strlen(buffer);

    while (1) {

        lock(lockName, buffer, bufferSize);

        sleep(1);

        unlock(lockName, bufferSize);

        LOCKS++;
    }

    return 0; 
}