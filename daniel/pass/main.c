#include <stdio.h>
#include <stdlib.h>
#include "ps.h"

int main() {
    process_t *process = (process_t *) calloc(sizeof(process_t), 1);
    int_array_t processes = getProcessIdList();
    if (!process) {
        fprintf(stderr, "couldn't allocate memory.");
    }
    for (int i = 0; i < processes.length; i++) {
        process->pid = processes.data[i];
        if (getProcessInfo(process)) {
            printf("pid:%d name:%s, path:%s command:%s\n", process->pid, process->name, process->path,
                   process->command);
        } else {
            printf("couldn't find the %d process name.", process->pid);
        }
    }
    free(process);
    return 0;
}
