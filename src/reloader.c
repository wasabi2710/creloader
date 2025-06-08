#include <stdio.h>
#include <string.h>
#include <windows.h>
#include "reloader.h"

void _init_reloader(const char* src_path, char* new_path) {
    const char* dot = strrchr(src_path, '.');
    if (dot) {
        size_t prefix_len = dot - src_path;
        strncpy(new_path, src_path, prefix_len);
        new_path[prefix_len] = '\0';  
    } else {
        strcpy(new_path, src_path);
    }

    strcat(new_path, ".dll");
}

void _reloader(const char* dll_path, FuncType* func_type) {
    printf("\n== Reloading ==\n");
    HMODULE dll = LoadLibraryA(dll_path);
    if (dll == NULL) {
        DWORD error = GetLastError();
        printf("_RELOADER: failed loading '%s' [Error %lu]\n", dll_path, error);
        return;
    }

    func_type->func = (void (*)(void))GetProcAddress(dll, func_type->func_name);
    if (!func_type->func) {
        printf("_RELOADER: '%s' function not found in DLL\n", func_type->func_name);
        FreeLibrary(dll);
        return;
    }

    func_type->func();  // call the func

    FreeLibrary(dll);
}
