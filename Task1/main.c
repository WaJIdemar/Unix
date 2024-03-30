#include <stdio.h>
#include <string.h>
#include <stdlib.h> 
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <getopt.h>

#define BLOCK_ARGUMENT "b"
#define ERROR_COUNT_ARGS "ERROR: Invalid count of args\n"
#define ERROR_INVALID_BLOCK_SIZE "ERROR: Invalid block size\n"
#define ERROR_INVALID_SOURCE "ERROR: Can't open SOURCE file\n"
#define ERROR_INVALID_TARGET "ERROR: Can't open TARGET file\n"
#define ERROR_READ_SOURCE "ERROR: Can't read SOURCE file\n"
#define ERROR_WRITE_TARGET "ERROR: Can't write TARGET file\n"
#define ERROR_LSEEK "ERROR: lseek error"
#define HELP_TIP "Use: myprogram [-"BLOCK_ARGUMENT" block_size] TARGET\nmyprogram [-"BLOCK_ARGUMENT" block_size] SOURCE TARGET\n"
#define BLOCK_SIZE 4096

void closeFD(int sourceFD, int targetFD) {
    close(sourceFD);
    close(targetFD);
}

void printErrorMessage(char* error) {
    printf("%s", error);
    printf(HELP_TIP);
}

void printErrorMessageWithClose(char* error, int sourceFD, int targetFD) {
    closeFD(sourceFD, targetFD);
    printf("%s", error);
    printf(HELP_TIP);
}

int openTarget(char* file) {
    return open(file, O_WRONLY | O_CREAT | O_TRUNC, 0664);
}

int openSource(char* file) {
    return open(file, O_RDONLY);
}

int main(int argc, char* argv[]) {

    int blockSize = BLOCK_SIZE;
    int targetFD, sourceFD;
    int param;

    while ((param = getopt(argc, argv, "b:")) != -1) {
        if (param == 'b') {
            blockSize = atoi(optarg);
        }
    }

    if (blockSize <= 0) {
        printErrorMessage(ERROR_INVALID_BLOCK_SIZE);
        return 1;
    }

    switch (argc - optind) {
        case 1:
            sourceFD = STDIN_FILENO;
            targetFD = openTarget(argv[optind]);
            break;

        case 2:
            sourceFD = openSource(argv[optind]);
            targetFD = openTarget(argv[++optind]);
            break;

        default:
            printErrorMessage(ERROR_COUNT_ARGS);
            return 1;
    }

    if (targetFD == -1) {
        printErrorMessageWithClose(ERROR_INVALID_TARGET, sourceFD, targetFD);
        return -1;
    }

    if (sourceFD == -1) {
        printErrorMessageWithClose(ERROR_INVALID_SOURCE, sourceFD, targetFD);
        return -1;
    }

    char buffer[blockSize];
    int isEmpty;
    int bufferSize, writeResult, lseekResult;

    while (bufferSize = read(sourceFD, buffer, blockSize)) {

        if (bufferSize == -1) {
            printErrorMessageWithClose(ERROR_READ_SOURCE, sourceFD, targetFD);
            return -1;
        }

        isEmpty = 1;
        for (int i = 0; i < bufferSize; i++) {
            if (buffer[i] != 0) {
                isEmpty = 0;
                break;
            }
        }

        if (isEmpty) {
            lseekResult = lseek(targetFD, bufferSize, SEEK_CUR);
            if (lseekResult == -1) {
                printErrorMessageWithClose(ERROR_LSEEK, sourceFD, targetFD);
                return -1;
            }
        } else {
            writeResult = write(targetFD, buffer, bufferSize);
            if (writeResult == -1) {
                printErrorMessageWithClose(ERROR_WRITE_TARGET, sourceFD, targetFD);
                return -1;
            }
        }
    }

    closeFD(sourceFD, targetFD);

    return 0;
}

