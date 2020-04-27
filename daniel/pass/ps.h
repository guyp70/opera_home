#ifndef PS_H
#define PS_H

#include <limits.h>

#define MAX_PID 419430
#define MAX_COMMAND 4096
#define MAX_PROC_PATH 30

/**
 * a struct representing a single process.
 */
typedef struct _Process {
    char name[500];
    int pid;
    char command[MAX_COMMAND];
    char path[PATH_MAX];
} Process;

typedef  struct  t_int_array {
    int length;
    int data[MAX_PID];
} int_array;

/**
 * The function gets a pid as an argument and returns info about a certain process.
 * @param pid the process id.
 * @return a process struct describing it's properties.
 */
Process getProcessInfo(int pid);

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
int_array getProcesses();

/**
 * the function gets a process struct and a pid and finds and updates the process's name.
 * @param process the process struct.
 * @param pid the process id.
 */
void getProcessName(Process *process, int pid);

/**
 * the function gets a process struct and a pid and finds and updates the process's command line.
 * @param process the process struct.
 * @param pid the process id.
 */
void getProcessCommand(Process *process, int pid);

/**
 * the function gets a process struct and a pid and finds and updates the process's path.
 * @param process the process struct.
 * @param pid the process id.
 */
void getProcessPath(Process *process, int pid);

#endif //PS_H
