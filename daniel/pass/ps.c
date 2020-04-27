#include "ps.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <dirent.h>

int_array getProcesses() {
    DIR *directory;
    int_array pidList;
    int counter = 0;
    struct dirent *file;
    directory = opendir("/proc");
    if (directory) {
        while ((file = readdir(directory)) != NULL) {
            if (atoi(file->d_name)) {
                pidList.data[counter++] = atoi(file->d_name);
            }
        }
        pidList.length = counter;
        closedir(directory);
    }
    return pidList;
}

void replace(char *string, int length, char old, char new) {
    for (int counter = 1; counter < length; counter++) {
        if (string[counter] == old) {
            string[counter] = new;
        }
    }
    string[length - 1] = 0;
}

void getProcessName(Process *process, int pid) {
    char statFileName[MAX_PROC_PATH];
    sprintf(statFileName, "/proc/%d/stat", pid);
    FILE *statFile = fopen(statFileName, "r");
    fscanf(statFile, "%*s %s", process->name);
}

void getProcessCommand(Process *process, int pid) {
    char cmdFileName[MAX_PROC_PATH];
    char *cmdData = (char *) calloc(MAX_COMMAND, sizeof(char));
    int copiedLength = 0;
    sprintf(cmdFileName, "/proc/%d/cmdline", pid);
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

void getProcessPath(Process *process, int pid) {
    char *binaryFile;
    char realPath[PATH_MAX + 1];
    sprintf(binaryFile, "/proc/%d/exe", pid);
    if (readlink(binaryFile, realPath, PATH_MAX) != -1) {
        strcpy(process->path, realPath);
    } else {
        strcpy(process->path, "null");
    }
}

Process getProcessInfo(int pid) {
    Process process;
    getProcessName(&process, pid);
    getProcessPath(&process, pid);
    getProcessCommand(&process, pid);
    process.pid = pid;

    return process;
}
