#include <stdio.h>
#include <sys/fanotify.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/select.h>
#include <libgen.h>
#include "my_ps/proc_info.h"

#define TIMEOUT_INTERVAL (1)
#define ERROR_DATA_OF_INCOMPATIBLE_SIZE (1)
#define ERROR_INOTIFY (2)
#define MONITORED_FS_MOUNT_POINT ("/")
#define MONITORED_BIN_NAME ("sleep")


static bool KEEP_RUNNING = true;

/*
 * Possible way of implementation:
 * Use fanotify to monitor access to all files in the fs. Then check for each event the exe symlink in the /proc folder.
 */

void int_handler(int sig) {
    //signal(SIGINT, int_handler);
    KEEP_RUNNING = false;
}

void reset_select_vars(struct timeval *timeout, fd_set *rfds, int file_fd) {
    timeout->tv_sec = TIMEOUT_INTERVAL;
    timeout->tv_usec = 0;
    FD_ZERO(rfds);
    FD_SET(file_fd, rfds);
}

/*
 * Prints an alert every time a process accesses a file with the name specified at MONITORED_BIN_NAME.
*/
void handle_event(struct fanotify_event_metadata *event) {
    char executed_file_path[PATH_MAX];
    char procfs_fd_path[PATH_MAX];
    ssize_t bytes_read;

    proc_info_t pi;
    if (0 != init_proc_info(&pi, event->pid)) {
        printf("Failed to get info on proc %d.\n", event->pid);
        return;
    }
    sprintf(procfs_fd_path, "/proc/self/fd/%d", (int)event->fd);
    bytes_read = readlink(procfs_fd_path, executed_file_path, PATH_MAX - 1);
    if (bytes_read == 0) {
        printf("Failed to readlink at %s. (Could not get accessed file procfs_fd_path)\n", procfs_fd_path);
        return;
    }
    executed_file_path[bytes_read] = '\0';
    if (0 == strcmp(MONITORED_BIN_NAME, basename(executed_file_path))) {
        printf(
                "%s binary has been executed.(PID: %d PROC NAME: \"%s\" BIN PATH: \"%s\")\n",
                MONITORED_BIN_NAME,
                event->pid,
                pi.name,
                executed_file_path
        );
    }
    free_proc_info(pi);
    close(event->fd);
}

int main() {
    // Inotify related vars
    int fanotify_events_fd;
    size_t bytes_read;
    struct fanotify_event_metadata event;

    // Set the interrupt handler.
    signal(SIGABRT, int_handler);

    // Select related vars
    struct timeval timeout;
    fd_set rfds;
    int select_retval;


    // Set the interrupt handler.
    signal(SIGINT, int_handler);

    // Open inotify event stream.
    fanotify_events_fd = fanotify_init(FAN_CLASS_NOTIF, O_RDONLY);
    if (fanotify_events_fd == -1) {
        printf("Error while calling fanotify_init().\n");
        return ERROR_INOTIFY;
    }
    if (fanotify_mark(fanotify_events_fd,FAN_MARK_ADD | FAN_MARK_MOUNT, FAN_ACCESS,0, MONITORED_FS_MOUNT_POINT) != 0) {
        printf("Error while calling fanotify_mark().\n");
        return ERROR_INOTIFY;
    }

    while (KEEP_RUNNING) {
        reset_select_vars(&timeout, &rfds, fanotify_events_fd);
        select_retval = select(fanotify_events_fd + 1, &rfds, NULL, NULL, &timeout);

        if (select_retval == 1 && FD_ISSET(fanotify_events_fd, &rfds)) {
            // Read event from inotify stream.
            bytes_read = read(fanotify_events_fd, (void *) &event, sizeof(struct fanotify_event_metadata));
            if (bytes_read != sizeof(struct fanotify_event_metadata)) {
                printf("Read data of incompatible size.\n");
                return ERROR_DATA_OF_INCOMPATIBLE_SIZE;
            }
            handle_event(&event);
        }
    }
    printf("Exiting...\n");
    close(fanotify_events_fd);
    return 0;
}
