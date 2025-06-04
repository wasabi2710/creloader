#include <stdio.h>
#include <stdlib.h>

#include "fftype.h"

int main(int argc, char* argv[]) {
    // force path selection
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <src>\n", argv[0]);
        return 1;
    }

    Paths paths = find_file_path_wrapper("src", ".c");

    while (paths.count > 0) {
        paths.count--;
        fprintf(stdout, "C file: %s\n", paths.paths[paths.count]);
        free(paths.paths[paths.count]);
    }

    free(paths.paths[paths.count]);

    return 0;
}