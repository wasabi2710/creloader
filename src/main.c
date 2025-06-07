#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <wingdi.h>
#include <winnt.h>

#define INIT_LINE_SIZE 1024
#define MAIN_ENTRY_STR "run_all"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define INIT_LINE_SIZE 1024
#define MAX_PATH 260

char* get_entry_filename(char fullpath[MAX_PATH]) {
    char* filename = NULL;
    char line[INIT_LINE_SIZE];

    FILE* fd = fopen(fullpath, "r");
    if (!fd) {
        fprintf(stderr, "GET_ENTRY_FILENAME: failed opening file\n");
        return NULL;
    }

    while (fgets(line, sizeof(line), fd)) {
        // Remove trailing newline if present
        size_t len = strlen(line);
        if (len > 0 && line[len - 1] == '\n') {
            line[len - 1] = '\0';
        }

        if (strstr(line, "run_all") != NULL) {
            // Found the string, allocate and return filename
            filename = malloc(strlen(fullpath) + 1);
            if (filename) {
                strcpy(filename, fullpath);
            }
            break;
        }
    }

    fclose(fd);
    return filename; 
}

char* find_main_entry(const char* src_path) {
    char search_path[MAX_PATH];
    snprintf(search_path, MAX_PATH, "%s\\*", src_path);

    char* entryfn = NULL;

    WIN32_FIND_DATAA found_data;
    HANDLE handle = FindFirstFileA(search_path, &found_data);

    if (handle == INVALID_HANDLE_VALUE) {
        DWORD error = GetLastError();
        if (error != ERROR_FILE_NOT_FOUND) {
            fprintf(stderr, "Error in %s (Code %lu)\n", src_path, error);
        }
        return NULL;
    }

    do {
        if (strcmp(found_data.cFileName, ".") == 0 || 
            strcmp(found_data.cFileName, "..") == 0) {
            continue;
        }

        char full_path[MAX_PATH];
        snprintf(full_path, MAX_PATH, "%s\\%s", src_path, found_data.cFileName);

        if (found_data.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY) {
            entryfn = find_main_entry(full_path);
            if (entryfn != NULL) {
                FindClose(handle);
                return entryfn;  // found in subdirectory, stop
            }
        } else {
            entryfn = get_entry_filename(full_path);
            if (entryfn != NULL) {
                FindClose(handle);
                return entryfn;  // found in file, stop
            } else {
                fprintf(stderr, "FIND_MAIN_ENTRY: run_all entry not found on %s\n", full_path);
            }
        }
    } while (FindNextFileA(handle, &found_data));

    FindClose(handle);
    return NULL;  
}

int main(int argc, char* argv[]) {
    if (argc < 2) { 
        fprintf(stderr, "Usage: %s <directory_path>\n", argv[0]); 
        return 1;
    }

    char* entry = find_main_entry(argv[1]);  

    printf("Main entry is from %s\n", entry);

    return 0;
}