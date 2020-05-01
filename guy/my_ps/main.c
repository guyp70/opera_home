#include <stdio.h>
#include <stdbool.h>
#include <dirent.h>
#include "proc_info.h"

#define ERROR_WHILE_PARSING_DIRENT (10)
#define ERROR_EXPLORING_PROC (11)

/*
Return 1 if the string contains only digits (0-9). 0 Otherwise.
(Returns True for empty string)
*/
bool str_is_digit(const char *s) {
    int i = 0;
    for (i = 0; '\0' != s[i]; ++i) {
        if (!isdigit(s[i])) {
            return false;
        }
    }
    return true;
}

void print_ps_line(char *name, unsigned int pid, char *cmd, char *exe_path) {
    printf("%-7d || %-30s || %-40s || %-40s\n", pid, name, cmd, exe_path);
}

int main() {
    DIR *proc_dir;
    struct dirent *dir_entry;
    proc_info_t pi;
    unsigned int pid;
    proc_dir = opendir(PROC_DIR_PATH);

    printf("%-7s || %-30s || %-40s || %-40s\n", "PID", "Name", "Cmd", "Exec Path");
    while (NULL != (dir_entry = readdir(proc_dir))) {
        // Checks that the dir_entry is a dir and that its name is numeric.
        // Both conditions are checked to insure the entry is a proc folder.
        if (dir_entry->d_type == DT_DIR && str_is_digit(dir_entry->d_name)) {
            if (1 != sscanf(dir_entry->d_name, "%d", &pid)) {
                if (DEBUG) {
                    printf("An Error has occurred while parsing: %s.\n\n", dir_entry->d_name);
                    return ERROR_WHILE_PARSING_DIRENT;
                }
                continue;
            }
            if (init_proc_info(&pi, pid)) {
                if (DEBUG) {
                    printf("An Error has occurred while exploring proc %s.\n\n", dir_entry->d_name);
                    return ERROR_EXPLORING_PROC;
                }
                continue;  //return (int) err;
            }
            print_ps_line(pi.name, pi.id, pi.cmd_line, pi.exe_path);
            free_proc_info(pi);
        }
    }
    closedir(proc_dir);
}
