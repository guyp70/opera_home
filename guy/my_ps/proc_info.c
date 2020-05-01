//
// Created by root on 4/24/20.
//

#include "proc_info.h"


/*
 * Copies the file's content in a buffer. Built to work with the proc fs.
 * path [IN] -> The file's path.
 * file_contents [OUT] -> buffer to which we copy the file's contents.
 * max_read_size [IN] -> Maximum size of data to read from file.
 * dereference_symlinks [IN] -> If true, read the file the symlink point to rather than the link file itself.
* RETURN -> Number of bytes read.
 */
size_t read_proc_fs_file(const char *path, char *file_contents, size_t max_read_size, bool dereference_symlinks) {
    size_t bytes_read = 0;
    struct stat file_stat;

    if (fstatat(0, path, &file_stat, AT_SYMLINK_NOFOLLOW)) {
        if (DEBUG) {
            printf("Couldn't stat \"%s\". Check it exists and has the right permissions set.\n", path);
        }

        file_contents = NULL;
        return 0;
    }
    // If told not to dereference links, check if the file is a link and if it is return its target.
    if (!dereference_symlinks && S_ISLNK(file_stat.st_mode)) {
        bytes_read = readlink(path, file_contents, PATH_MAX - 1);
        file_contents[bytes_read] = '\0';
        return bytes_read;
    }

    FILE *fd = fopen(path, "r");
    if (NULL == fd) {
        if (DEBUG) {
            printf("An Error has occurred while opening \"%s\".\n", path);
        }
    } else {
        // Since it turns out we cant use lseek or stat to find out file size, we will to read
        //bytes_read = fread((void*)file_contents, MAX_PROC_FS_FILE_SIZE, 1, fd);
        for (bytes_read = 0;
             bytes_read < max_read_size && EOF != (file_contents[bytes_read] = (char)fgetc(fd)); bytes_read++);
    }
    fclose(fd);
    return bytes_read;
}

/*
 * CANT HANDLE LINKS!!!
 * Copies the file's content in a buffer. Built to work with the proc fs.
 * proc_id [IN] -> Process ID.
 * file_name [IN] -> The name of the file to read. (relative to a /proc/x/ dir.
 * file_contents [OUT] -> buffer to which we copy the file's contents.
 * max_read_size [IN] -> Maximum size of data to read from file.
* RETURN -> Number of bytes read.
 */
size_t get_proc_file_contents(const unsigned int proc_id, const char *file_name, char *file_contents, size_t  max_read_size) {
    size_t bytes_read = 0;
    char *file_path;
    file_path = (char *) malloc(PATH_MAX);
    sprintf(file_path, "%s/%d/%s", PROC_DIR_PATH, proc_id, file_name);
    bytes_read = read_proc_fs_file(file_path, file_contents, max_read_size, false);
    if (!bytes_read) {
        if (DEBUG) {
            printf("An Error has occurred while reading file \"%s\" of proc %d.\n", file_path, proc_id);
        }
    }
    free((void *) file_path);
    return bytes_read;
}

/*
 * Initialize a process info struct and load it with relevant data.
 *
 * *pi [OUT] -> Pointer to a struct to initialize.
 * pid [IN] -> The PID of process.
 * RETURNS -> Error Code.
 */
unsigned int init_proc_info(proc_info_t *pi, const unsigned int pid) {
    size_t bytes_read = 0;
    unsigned int i = 0;
    pi->id = pid;

    // Allocate memory buffers for name, command line and exec path.
    // (Set last byte to \0 in case the string is longer than MAX_PROC_FS_FILE_SIZE)
    pi->name = (char *) malloc(MAX_PROC_FS_FILE_SIZE + 1);
    pi->name[MAX_PROC_FS_FILE_SIZE] = '\0';
    pi->cmd_line = (char *) malloc(MAX_PROC_FS_FILE_SIZE + 1);
    pi->cmd_line[MAX_PROC_FS_FILE_SIZE] = '\0';
    pi->exe_path = (char *) malloc(MAX_PROC_FS_FILE_SIZE + 1);
    pi->exe_path[MAX_PROC_FS_FILE_SIZE] = '\0';

    if (NULL == pi->name || NULL == pi->cmd_line || NULL == pi->exe_path) {
        printf("An Error has occurred while allocating memory.");
        return ERROR_WHILE_ALLOCATING_MEMORY;
    }

    bytes_read = get_proc_file_contents(pid, "comm", (pi->name), MAX_PROC_FS_FILE_SIZE);
    if (!bytes_read) {
        if (DEBUG) {
            printf("An Error has occurred while extracting proc %d comm name.\n", pid);
            return ERROR_WHILE_GETTING_COMM;
        }
        strcpy(pi->name, UNKNOWN_STR); //return ERROR_WHILE_GETTING_COMM;
    }
    (pi->name)[bytes_read - 1] = '\0';
    bytes_read = get_proc_file_contents(pid, "cmdline", (pi->cmd_line), MAX_PROC_FS_FILE_SIZE);
    if (!bytes_read) {
        if (DEBUG) {
            printf("An Error has occurred while extracting proc %d cmd.\n", pid);
            return ERROR_WHILE_GETTING_CMD;
        }
        strcpy(pi->cmd_line, UNKNOWN_STR); //return ERROR_WHILE_GETTING_CMD;
    } else {
        // The cmd line is kept as several strings. One for each arg.
        // We therefor have to turn the separate strings into one continuous one.
        for (i = 0; i < bytes_read - 1; i++) {
            if (pi->cmd_line[i] == '\0') {
                pi->cmd_line[i] = ' ';
            }
        }
    }
    bytes_read = get_proc_file_contents(pid, "exe", (pi->exe_path), MAX_PROC_FS_FILE_SIZE);
    if (!bytes_read) {
        if (DEBUG) {
            printf("An Error has occurred while extracting proc %d exe.\n", pid);
            return ERROR_WHILE_GETTING_EXE_PATH;
        }
        strcpy(pi->exe_path, UNKNOWN_STR); //return ERROR_WHILE_GETTING_EXE_PATH;
    }
    return 0;
}

// Clean a proc_info_t struct.
void free_proc_info(proc_info_t pi) {
    free((void *) pi.cmd_line);
    free((void *) pi.name);
    free((void *) pi.exe_path);
}