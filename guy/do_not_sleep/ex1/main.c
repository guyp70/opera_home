#include <stdio.h>
#include <sys/inotify.h>
#include <stdbool.h>
#include <unistd.h>
#include <signal.h>
#include <sys/select.h>

#define TIMEOUT_INTERVAL (1)
#define ERROR_DATA_OF_INCOMPATIBLE_SIZE (1)
#define ERROR_INOTIFY (2)
#define SLEEP_BIN_PATH ("/bin/sleep")

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

void handle_event(struct inotify_event *event, int sleep_watch_descriptor) {
    // Print alert if is it related to the file we track (/bin/sleep).
    if (event->wd == sleep_watch_descriptor) {
        // I could use the event.mask to figure out what was done but I'm lazy so let's just say we are satisfied
        // with this PoC :)
        printf("%s has been accessed.\n", SLEEP_BIN_PATH);
    }
}

int main() {

    // Inotify related vars
    int inotify_events_fd;
    int sleep_watch_descriptor;
    size_t bytes_read;
    struct inotify_event event;

    // Select related vars
    struct timeval timeout;
    fd_set rfds;
    int select_retval;


    // Set the interrupt handler.
    signal(SIGINT, int_handler);

    // Open inotify event stream.
    inotify_events_fd = inotify_init();
    if (inotify_events_fd == -1) {
        printf("Error while calling inotify_init().\n");
        return ERROR_INOTIFY;
    }
    sleep_watch_descriptor = inotify_add_watch(inotify_events_fd, SLEEP_BIN_PATH, IN_ACCESS);

    if (sleep_watch_descriptor == -1) {
        printf("Error while calling inotify_add_watch().\n");
        return ERROR_INOTIFY;
    }
    while (KEEP_RUNNING) {
        reset_select_vars(&timeout, &rfds, inotify_events_fd);
        select_retval = select(inotify_events_fd + 1, &rfds, NULL, NULL, &timeout);

        if (select_retval == 1 && FD_ISSET(inotify_events_fd, &rfds)) {
            // Read event from inotify stream.
            bytes_read = read(inotify_events_fd, (void *) &event, sizeof(struct inotify_event));
            if (bytes_read != sizeof(struct inotify_event)) {
                printf("Read data of incompatible size.\n");
                return ERROR_DATA_OF_INCOMPATIBLE_SIZE;
            }
            handle_event(&event, sleep_watch_descriptor);
        }
    }
    printf("Exiting...\n");
    close(inotify_events_fd);
    return 0;
}
