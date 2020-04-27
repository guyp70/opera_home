#include <stdio.h>
#include "ps.h"

int main() {
    int i;
    Process process;
    int_array processes = getProcesses();
    for (i = 0; i < processes.length; i++) {
    process = getProcessInfo(processes.data[i]);
    printf("pid:%d name:%s, path:%s command:%s\n", process.pid, process.name, process.path, process.command);
    }
    return 0;
}
