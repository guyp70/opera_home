//
// Created by sarne on 4/26/20.
//

#ifndef PS_H
#define PS_H

#include <limits.h>
#include <stdlib.h>

#define MAX_PID 419430
#define MAX_COMMAND 4096
#define MAX_PROC_PATH 30

/**
 * a struct representing a single process.
 */
typedef struct __process_t {
    char name[500];
    pid_t pid;
    char command[MAX_COMMAND];
    char path[PATH_MAX];
} process_t;

typedef struct __int_array {
    int length;
    pid_t data[MAX_PID];
} int_array_t;

/**
 * The function gets a pid as an argument and returns info about a certain process.
 * @param process a process to get the info about
 * @return a positive value if the function was successful and 0 if it failed.
 */
int getProcessInfo(process_t *process);

/**
 * the function replaces all appearances of old char with new char for the following length chars
 * @param string the string that should be manipulated.
 * @param length the length of the string.
 * @param old the char that needs to be replaced.
 * @param new the char that needs to be replaced with.
 */
void replace(char *string, int length, char old, char new);

/**
 * the function returns the list of processes.
 * @return the list of processes.
 */
int_array_t getProcessIdList();

/**
 * the function a process struct and a pid and finds and updates the process's name.
 * @param process the process struct.
 * @param pid the process id.
 */
int getProcessName(process_t *process);

/**
 * the function gets a process struct and a pid and finds and updates the process's command line.
 * @param process the process struct.
 * @param pid the process id.
 */
void getProcessCommand(process_t *process);

/**
 * the function gets a process struct and a pid and finds and updates the process's path.
 * @param process the process struct.
 * @param pid the process id.
 */
void getProcessPath(process_t *process);

#endif //PS_H
