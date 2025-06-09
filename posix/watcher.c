#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <sys/inotify.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>

int is_cfile(const char *filename) {
    const char *dot = strrchr(filename, '.');
    return dot && strcmp(dot, ".c") == 0;
}

// func: recursively add watch to all dirs
void add_mon(const char *src_dir, int notifd) {
    fprintf(stdout, "MON_DIR: watching %s\n", src_dir);
    int wd = inotify_add_watch(notifd, src_dir, IN_MODIFY);
    if (wd == -1) {
        fprintf(stderr, "MON_DIR: failed add_watch to source dir\n");
        return;
    }

    DIR *dirstream = opendir(src_dir);
    if (!dirstream) {
        fprintf(stderr, "SRC_WATCHER: failed opening directory stream: %s\n", src_dir);
        return;
    }

    struct dirent *dirp;
    // while loop blocks 
    while ((dirp = readdir(dirstream)) != NULL) {
        // skip "." and ".."
        if (strcmp(dirp->d_name, ".") == 0 || strcmp(dirp->d_name, "..") == 0) {
            continue;
        }

        char full_path[PATH_MAX];
        snprintf(full_path, sizeof(full_path), "%s/%s", src_dir, dirp->d_name);

        struct stat statbuf;
        if (stat(full_path, &statbuf) == -1) {
            fprintf(stderr, "SRC_WATCHER: failed stat on %s\n", full_path);
            continue;
        }

        if (S_ISDIR(statbuf.st_mode)) {
            // recurse
            add_mon(full_path, notifd);
        } 
    }

    closedir(dirstream);
}

// func: process mon events
void process_mon_events(int notifd, char* src_dir) {
    char buf[4096];
    while (1) {
        ssize_t br = read(notifd, buf, sizeof(buf));
        if (br == -1) {
            fprintf(stderr, "MON_DIR: failed reading notify buffer\n");
            break;
        }

        for (char *ptr = buf; ptr < buf + br; ) {
            struct inotify_event *event = (struct inotify_event *)ptr;

            if ((event->mask & IN_MODIFY) && is_cfile(event->name)) {
                fprintf(stdout, "File modified: %s\n", event->name);
                // reserve: for calling reloader and obj comp
            }

            ptr += sizeof(struct inotify_event) + event->len;
        }
    }
}

// func: call mon_dir on base_path & recursively apply calling of mon_dir to subdirs
void src_watcher(const char *base_path) {
    // init notifd
    int notifd = inotify_init();
    if (notifd == -1) {
        fprintf(stderr, "MON_DIR: failed initializing inotify\n");
        return;
    }

    // recursively add all watches
    add_mon(base_path, notifd);

    // process events
    process_mon_events(notifd, (char*)base_path);

    // cleanup
    close(notifd);

}
