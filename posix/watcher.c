#include "watcher.h"
#include "processor.h"
#include "reloader.h"

void add_cmon(const char *src_dir, int notifd) {
    char path_cpy[PATH_MAX];
    strncpy(path_cpy, src_dir, sizeof(path_cpy) - 1);

    char *build_path = dirname(path_cpy);

    //cmakelists.txt
    char cmake_path[PATH_MAX];
    snprintf(cmake_path, PATH_MAX, "%s/%s", build_path, "CMakeLists.txt");

    fprintf(stdout, "MON_DIR: Cmake watching %s\n", cmake_path);
    int wd = inotify_add_watch(notifd, cmake_path, IN_MODIFY);
    if (wd == -1) {
        fprintf(stderr, "MON_DIR: failed add_watch to CmakeList\n");
        return;
    }
}

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

//src files guard
int is_cfile(const char *filename) {
    const char *dot = strrchr(filename, '.');
    if (!dot) return 0;  
    return (strcmp(dot, ".c")) == 0 || (strcmp(dot, ".h") == 0);
}

//cmake guard
int is_bpath(const char* base_path, const char* filename) {
    char tmp[PATH_MAX];
    strncpy(tmp, base_path, sizeof(tmp));
    tmp[sizeof(tmp)-1] = '\0';  

    char* last_slash = strrchr(tmp, '/');
    if (last_slash != NULL) {
        *last_slash = '\0';
    }

    printf("BPATH: tmp path %s\n", tmp);
    return (strcmp(tmp, filename) == 0);
}

unsigned long now_ms() {
    struct timeval tv;
    gettimeofday(&tv, NULL);
    return tv.tv_sec * 1000UL + tv.tv_usec / 1000;
}

void process_mon_events(int notifd, const char* src_dir) {
    char buf[4096];

    unsigned long last = 0;
    int debounce_time = 4000; //4secs

    while (1) {
        unsigned long current = now_ms();
        ssize_t br = read(notifd, buf, sizeof(buf));
        if (br == -1) {
            fprintf(stderr, "MON_DIR: failed reading notify buffer\n");
            break;
        }

        for (char *ptr = buf; ptr < buf + br; ) {
            struct inotify_event *event = (struct inotify_event *)ptr;

            if ((event->mask & IN_MODIFY) && ((current - last) >= debounce_time)) {
                printf("Event name: %s\n", event->name);
                if (is_cfile(event->name)) {
                    fprintf(stdout, "\n=== C File modified: %s ===\n", event->name);
                    process_cmake(src_dir, 0);
                } else {
                    fprintf(stdout, "\n=== CMakeLists modified ===\n");
                    process_cmake(src_dir, 1);
                }

                last = current;

                //find_sofile and reload
                const char* sofile = find_sofile(src_dir);
                reloader(find_sofile(src_dir));

                usleep(10000); //await 10ms
            }

            ptr += sizeof(struct inotify_event) + event->len;
        }
    }
}

void init(const char* base_path) {
    fprintf(stdout, "\n=== Initializing Cmake Build ===\n");
    process_cmake(base_path, 1);
    const char* sofile = find_sofile(base_path);
    reloader(find_sofile(base_path));
    fprintf(stdout, "\n=== Initializing Cmake Build ===\n\n");
}

void src_watcher(const char *base_path) {
    // init notifd
    int notifd = inotify_init();
    if (notifd == -1) {
        fprintf(stderr, "MON_DIR: failed initializing inotify\n");
        return;
    }

    //init
    init(base_path);

    //recursively add all watches
    add_mon(base_path, notifd);
    add_cmon(base_path, notifd);

    //process events
    process_mon_events(notifd, (char*)base_path);

    //cleanup
    close(notifd);
}
