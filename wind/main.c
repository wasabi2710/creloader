#include "watcher.h"

int main(int argc, char* argv[]) {
    if (argc < 2) {
        fprintf(stderr, "Usage: %s <src>\n", argv[0]);
        return 1;
    }
    //sani path
    const char* base_path = argv[1];

    watch_directory(base_path);
    // src_watcher procedure
    // if changes
    // => cmakelists: process_cmake(is_rebuild)
    // => src files: process_cmake(!is_rebuild)
    // => find_sofile()
    // => reloader(found_sofile)
}

