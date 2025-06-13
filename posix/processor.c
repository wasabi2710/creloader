#include "processor.h"
#include <linux/limits.h>
#include <errno.h>
#include <string.h>
#include <time.h>

// using cmake only
// !cmake must compile a shared object
void process_cmake(const char* src_dir, int rebuild) {
    // tmp returns 1 lvl dir above
    char tmp[PATH_MAX];
    strncpy(tmp, src_dir, sizeof(tmp));
    tmp[sizeof(tmp)-1] = '\0';

    char* last_slash = strrchr(tmp, '/');
    if (last_slash != NULL) {
        *last_slash = '\0';
    }

    char generator[1024];
    snprintf(generator, sizeof(generator), "cmake -S %s -B %s/build", tmp, tmp);

    char resourcer[1024];
    snprintf(resourcer, sizeof(resourcer), "cmake --build %s/build", tmp);

    char cmd[1024];
    if (rebuild) {
        snprintf(cmd, sizeof(cmd), "%s && %s", generator, resourcer);
    } else {
        snprintf(cmd, sizeof(cmd), "%s", resourcer);
    }

    fprintf(stdout, "Running: %s\n", cmd);
    int ret = system(cmd);
    if (ret != 0) {
        fprintf(stderr, "Command failed with code: %d\n", ret);
    }
}

// find sofile
// dependee: reloader(char* sofile)
char* find_sofile(const char* src_dir) {    
    // tmp returns above lvl dir
    char tmp[PATH_MAX];
    strncpy(tmp, src_dir, sizeof(tmp));
    tmp[sizeof(tmp)-1] = '\0';

    char* last_slash = strrchr(tmp, '/');
    if (last_slash != NULL) {
        *last_slash = '\0';
    }

    char path[PATH_MAX];
    snprintf(path, PATH_MAX, "%s/build", tmp);

    DIR* dir = opendir(path);
    if (!dir) {
        fprintf(stderr, "FIND_SOFILE: failed opening build directory '%s': %s\n", path, strerror(errno));
        return NULL;
    }

    struct dirent* entry;
    char* sofile = NULL;

    while ((entry = readdir(dir))) {
        if (entry->d_type != DT_REG) continue; 
        
        char* ext = strrchr(entry->d_name, '.');
        if (ext && strcmp(ext, ".so") == 0) {
            sofile = malloc(strlen(path) + strlen(entry->d_name) + 2);
            sprintf(sofile, "%s/%s", path, entry->d_name);
            break;
        }
    }

    closedir(dir);
    return sofile; 
}