#include "watcher.h"

#include <stdio.h>

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <src>\n", argv[0]);
        return 1;
    }
    // sani path
    const char* base_path = argv[1];

    src_watcher(base_path);

}

