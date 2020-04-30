#include "ps.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <errno.h>
#include <dirent.h>

#define EXE_PATH_STRING "/proc/%d/exe"
#define STAT_PATH_STRING "/proc/%d/stat"
#define CMD_PATH_STRING "/proc/%d/cmdline"

int_array_t getProcessIdList() {
    DIR *directory;
    int_array_t pidList;
    int counter = 0;
    struct dirent *file;
    directory = opendir("/proc");
    if (directory) {
        file = readdir(directory);
        while (file != NULL) {
            if (atoi(file->d_name)) {
                pidList.data[counter++] = atoi(file->d_name);
            }
            file = readdir(directory);
        }
        pidList.length = counter;
        closedir(directory);
    }
    return pidList;
}

void replace(char *string, int length, char old, char new) {
    for (int counter = 0; counter < length; counter++) {
        if (string[counter] == old) {
            string[counter] = new;
        }
    }
    string[length - 1] = 0;
}

int getProcessName(process_t *process) {
    char statFileName[MAX_PROC_PATH];
    sprintf(statFileName, STAT_PATH_STRING, process->pid);
    FILE *statFile = fopen(statFileName, "r");
    return fscanf(statFile, "%*s %s", process->name);
}


void getProcessCommand(process_t *process) {
    char cmdFileName[MAX_PROC_PATH];
    char *cmdData = (char *) calloc(MAX_COMMAND, sizeof(char));
    int copiedLength = 0;
    if (!cmdData) {
        fprintf(stderr, "error allocating memory.");
        exit(ENOMEM);
    }
    sprintf(cmdFileName, CMD_PATH_STRING, process->pid);
    FILE *fdCmdLine = fopen(cmdFileName, "r");

    if (fdCmdLine) {
        copiedLength = fread((void *) cmdData, sizeof(char), MAX_COMMAND, fdCmdLine);
        fclose(fdCmdLine);
        if (copiedLength) {
            replace(cmdData, copiedLength, '\0', ' ');
            strcpy(process->command, cmdData);
        } else {
            strcpy(process->command, "null");
        }
    } else {
        strcpy(process->command, "null");
    }
    free(cmdData);
}

void getProcessPath(process_t *process) {
    char *binaryFile;
    char realPath[PATH_MAX + 1];
    sprintf(binaryFile, EXE_PATH_STRING, process->pid);
    ssize_t read_link_successfully = readlink(binaryFile, realPath, PATH_MAX);
    if (read_link_successfully != -1) {
        strcpy(process->path, realPath);
    } else {
        strcpy(process->path, "null");
    }
}

int getProcessInfo(process_t *process) {
    int found_process_name = getProcessName(process);
    getProcessPath(process);
    getProcessCommand(process);
    if (found_process_name == EOF || !found_process_name) {
        return 0;
    }
    return 1;
}

