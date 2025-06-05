#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

#include "fftype.h"
#include "reloc.h"

int test() {
    // force path selection
    // if (argc < 2) {
    //     fprintf(stderr, "Usage: %s <src>\n", argv[0]);
    //     return 1;
    // }

    Paths paths = find_file_path("src", ".c");

    while (paths.count > 0) {
        paths.count--;
        fprintf(stdout, "C file: %s\n", paths.paths[paths.count]);
        free(paths.paths[paths.count]);
    }
    free(paths.paths);

    return 0;
}

Paths find_file_path(const char* base, const char* fType) {
    Paths paths = {NULL, 0};

    DIR* dirstream = opendir(base);
    if (dirstream == NULL) {
        fprintf(stderr, "Failed opening directory stream: %s\n", base);
        return paths;        
    }

    struct dirent* entry;
    while ((entry = readdir(dirstream)) != NULL) {
        // entry: '.' and ".."
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        char path[4096];
        snprintf(path, sizeof(path), "%s/%s", base, entry->d_name);

        // recurse if DT is dir
        if (entry->d_type == DT_DIR) {
            Paths subpaths = find_file_path(path, fType);
            for (size_t i = 0; i < subpaths.count; i++) {
                append_string(&paths.paths, &paths.count, subpaths.paths[i]);
            }
            // free subpaths
            for (size_t i = 0; i < subpaths.count; i++) {
                free(subpaths.paths[i]);
            }
            free(subpaths.paths);
        }
        // check if source files
        else if (strstr(entry->d_name, fType) != NULL) {
            append_string(&paths.paths, &paths.count, path);
        }
    }

    closedir(dirstream);
    return paths;
}