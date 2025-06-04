#include <stdio.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>

#include "fftype.h"
#include "reloc.h"

// source files recursion finder
// returns: char**
//

// Paths find_file_path(const char* base, const char* fType) {
//     Paths paths;

//     DIR* dirstream = opendir(base);
//     if (dirstream == NULL) {
//         fprintf(stderr, "Failed opening directory stream: %s\n", base);
//         return (Paths){0};        
//     }

//     struct dirent* entry;
//     while ((entry = readdir(dirstream)) != NULL) {
//         // entry: '.' and ".."
//         if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
//             continue;
//         }

//         char path[4096];
//         snprintf(path, sizeof(path), "%s/%s", base, entry->d_name);

//         // recurse if DT is dir
//         if (entry->d_type == DT_DIR) {
//            find_file_path(path, fType);
//         }
//         // check if source files
//         else if (strstr(entry->d_name, fType) != NULL) {
//             //printf("%s\n", path);
//             append_string(&paths.paths, &paths.count, path);
//         }
//     }

//     closedir(dirstream);
//     return paths;
// }