#include <stdio.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <limits.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <file>\n", argv[0]);
        return 1;
    }

    int fd = inotify_init();
    int wd = inotify_add_watch(fd, argv[1], IN_MODIFY | IN_DELETE_SELF);

    char buf[4096] __attribute__((aligned(__alignof__(struct inotify_event))));
    const struct inotify_event *event;

    while (1) {
        ssize_t len = read(fd, buf, sizeof(buf));
        if (len <= 0) break;

        for (char *ptr = buf; ptr < buf + len; ptr += sizeof(struct inotify_event) + event->len) {
            event = (const struct inotify_event *) ptr;
            if (event->mask & IN_MODIFY) {
                printf("File modified! (Check new size)\n");
            }
            if (event->mask & IN_DELETE_SELF) {
                printf("File deleted! Exiting.\n");
                return 0;
            }
        }
    }

    close(fd);
    return 0;
}
