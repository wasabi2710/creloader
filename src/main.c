#include <minwindef.h>
#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <wingdi.h>
#include <winnt.h>

#include "reloader.h"
#include "watcher.h"

#define INIT_LINE_SIZE 1024
#define MAIN_ENTRY_STR "run_all"

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

void compile(const char* source_file) {
    char source_file_copy[MAX_PATH]; 
    strcpy(source_file_copy, source_file); 

    // rm ext
    char* last_dot = strrchr(source_file_copy, '.');
    if (last_dot) *last_dot = '\0';  

    // build compile command
    char cmd[512];  
    snprintf(cmd, sizeof(cmd), 
        "gcc -fPIC -shared -o \"%s.dll\" \"%s.c\"",  
        source_file_copy,  
        source_file_copy); 

    STARTUPINFO si = { sizeof(STARTUPINFO) };
    PROCESS_INFORMATION pi;

    snprintf(
        cmd, 
        sizeof(cmd),
        "gcc -fPIC -shared -o %s.dll %s", 
        source_file_copy,
        source_file);
    
    if (!CreateProcess(
        NULL,  
        cmd,       
        NULL,
        NULL, 
        FALSE,   
        0,       
        NULL,      
        NULL, 
        &si,       
        &pi 
    )) {
        printf("Failed to start GCC (Error %lo)\n", GetLastError());
        return;
    }
    
    // Wait for compilation to finish
    WaitForSingleObject(pi.hProcess, INFINITE);
    
    DWORD exit_code;
    GetExitCodeProcess(pi.hProcess, &exit_code);
    
    if (exit_code == 0) {
        printf("Compilation successful!\n");
    } else {
        printf("Compilation failed!\n");
    }

    // close threads
    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

void obj_comp(const char* filepath) {
    // extract fn from path
    const char* filename = filepath;
    const char* ff = strrchr(filepath, '\\');
    if (!ff) {
        ff = strrchr(filepath, '/');
    }
    if (ff) {
        filename = ff + 1; 
    }

    printf("OBJ_COMP: %s to be comp\n", filename);

    // compile to dll
    // fork gcc pid
    compile(filepath);
}

// obj_comp void* ptr wrap
void _obj_comp_wrap(void* arg) {
    struct {
        char* entry;
    }* params = (typeof(params))arg;
    obj_comp(params->entry);
}

// _reloader void* ptr wrap
void _reloader_wrap(void* arg) {
    struct {
        char* obj_path;
        FuncType* run_all;
    }* params = (typeof(params))arg;
    _reloader(params->obj_path, params->run_all);
}

// even trigger
// int mfdsafsn(int argc, char* argv[]) {
//     if (argc < 2) { 
//         fprintf(stderr, "Usage: %s <directory_path>\n", argv[0]); 
//         return 1;
//     }

//     char* entry = find_main_entry(argv[1]);  
//     printf("Main entry is from %s\n", entry);

//     wchar_t dirpath[MAX_PATH];
//     MultiByteToWideChar(CP_UTF8, 0, argv[1], -1, dirpath, MAX_PATH);

//     wprintf(L"Directory path: %ls\n", dirpath);

//     char obj_path[MAX_PATH];
//     _init_reloader(entry, obj_path);

//     FuncType run_all = { "run_all", NULL };
//     FuncPtr func[2] = { _obj_comp_wrap, _reloader_wrap };

//     // init compilers
//     obj_comp(entry);
//     _reloader(obj_path, &run_all);

//     // main prog
//     watch_src_dir(dirpath, func, 2);

//     return 0;
// }

int main(int argc, char* argv[]) {
    if (argc < 2) { 
        fprintf(stderr, "Usage: %s <directory_path>\n", argv[0]); 
        return 1;
    }

    char* entry = find_main_entry(argv[1]);  
    printf("Main entry is from %s\n", entry);

    wchar_t dirpath[MAX_PATH];
    MultiByteToWideChar(CP_UTF8, 0, argv[1], -1, dirpath, MAX_PATH);

    wprintf(L"Directory path: %ls\n", dirpath);

    char obj_path[MAX_PATH];
    _init_reloader(entry, obj_path);

    // Prepare arguments for each function
    // Create structs matching what your wrappers expect
    struct {
        char* entry;
    } obj_comp_args = { entry };

    struct {
        char* obj_path;
        FuncType* run_all;
    } reloader_args = { obj_path, &(FuncType){ "run_all", NULL } };

    // Create array of function pointers and their arguments
    FuncPtr func[2] = { _obj_comp_wrap, _reloader_wrap };
    void* args[2] = { &obj_comp_args, &reloader_args };

    // Initial compilation and loading
    // obj_comp(entry);
    // _reloader(obj_path, reloader_args.run_all);
    func[0](args[0]);
    func[1](args[1]);

    // Start watching with proper arguments
    watch_src_dir(dirpath, func, args, 2);

    return 0;
}
