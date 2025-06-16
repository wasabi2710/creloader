#include "processor.h"

void process_cmake(const char* src_dir, int rebuild) {
    char tmp[MAX_PATH];
    strncpy(tmp, src_dir, sizeof(tmp));
    tmp[sizeof(tmp)-1] = '\0';

    char* last_slash = strrchr(tmp, '\\');
    if (!last_slash) last_slash = strrchr(tmp, '/');
    if (last_slash) *last_slash = '\0';

    char generator[1024];
    snprintf(generator, sizeof(generator), "cmake -S \"%s\" -B \"%s\\build\"", tmp, tmp);

    char resourcer[1024];
    snprintf(resourcer, sizeof(resourcer), "cmake --build \"%s\\build\"", tmp);

    char cmd[2048];
    if (rebuild) {
        snprintf(cmd, sizeof(cmd), "cmd.exe /C %s && %s", generator, resourcer);
    } else {
        snprintf(cmd, sizeof(cmd), "%s", resourcer);
    }

    printf("Running: %s\n", cmd);

    STARTUPINFOA si = { sizeof(si) };
    PROCESS_INFORMATION pi;

    if (!CreateProcessA(NULL, cmd, NULL, NULL, FALSE, CREATE_NO_WINDOW, NULL, NULL, &si, &pi)) {
        fprintf(stderr, "CreateProcess failed: %lu\n", GetLastError());
        return;
    }

    WaitForSingleObject(pi.hProcess, INFINITE);

    DWORD exit_code;
    GetExitCodeProcess(pi.hProcess, &exit_code);
    if (exit_code != 0) {
        fprintf(stderr, "Command failed with code: %lu\n", exit_code);
    }

    CloseHandle(pi.hProcess);
    CloseHandle(pi.hThread);
}

char* find_sofile(const char* src_dir) {
    char tmp[MAX_PATH];
    strncpy(tmp, src_dir, sizeof(tmp));
    tmp[sizeof(tmp)-1] = '\0';

    char* last_slash = strrchr(tmp, '\\');
    if (!last_slash) last_slash = strrchr(tmp, '/');
    if (last_slash) *last_slash = '\0';

    char path[MAX_PATH];
    snprintf(path, sizeof(path), "%s\\build", tmp);

    char search_pattern[MAX_PATH];
    snprintf(search_pattern, sizeof(search_pattern), "%s\\*.dll", path);

    WIN32_FIND_DATAA fd;
    HANDLE h = FindFirstFileA(search_pattern, &fd);
    if (h == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "FIND_SOFILE: failed opening build directory '%s'\n", path);
        return NULL;
    }

    char* dll_path = NULL;

    do {
        if (!(fd.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY)) {
            dll_path = malloc(strlen(path) + strlen(fd.cFileName) + 2);
            sprintf(dll_path, "%s\\%s", path, fd.cFileName);
            break;
        }
    } while (FindNextFileA(h, &fd));

    FindClose(h);
    return dll_path;
}
