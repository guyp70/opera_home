//
// Created by root on 4/24/20.
//

#ifndef MY_PS_PROC_INFO_H
#define MY_PS_PROC_INFO_H

#include <stdlib.h>
#include <stdio.h>
#include <values.h>
#include "proc_info.h"
#include <ctype.h>
#include <sys/stat.h>
#include <stdbool.h>
#include <unistd.h>
#include <fcntl.h>
#include <string.h>

#define PROC_DIR_PATH ("/proc")
#define MAX_PROC_FS_FILE_SIZE (4096) // in bytes
#define ERROR_WHILE_GETTING_COMM (1)
#define ERROR_WHILE_GETTING_CMD (2)
#define ERROR_WHILE_GETTING_EXE_PATH (3)
#define ERROR_WHILE_ALLOCATING_MEMORY (4)
#define ERROR_STAT_PERMISSIONS (5)
#define UNKNOWN_STR ("???")
#define DEBUG (0)


typedef struct _proc_info_t {
    unsigned int id;
    char *name;
    char *cmd_line;
    char *exe_path;
} proc_info_t;


/*
 * Initialize a process info struct and load it with relevant data.
 *
 * *pi [OUT] -> Pointer to a struct to initialize.
 * pid [IN] -> The PID of process.
 * RETURNS -> Error Code.
 */
unsigned int init_proc_info(proc_info_t *pi, const unsigned int pid);


// Clean a proc_info_t struct.
void free_proc_info(proc_info_t pi);

#endif //MY_PS_PROC_INFO_H
