#include "watcher.h"
#include "processor.h"
#include "reloader.h"

int is_cfile(const char* filename) {
    const char* dot = strrchr(filename, '.');
    if (!dot) return 0;
    return strcmp(dot, ".c") == 0 || strcmp(dot, ".h") == 0;
}

unsigned long now_ms() {
    struct timeb t;
    ftime(&t);
    return t.time * 1000 + t.millitm;
}

void init(const char* base_path) {
    fprintf(stdout, "\n=== Initializing CMake Build ===\n");
    process_cmake(base_path, 1);
    char* sofile = find_sofile(base_path);
    if (sofile) {
        reloader(sofile);
        free(sofile);
    }
    fprintf(stdout, "\n=== Initialization complete ===\n\n");
}

void watch_directory(const char* base_path) {
    wchar_t wdir[MAX_PATH];
    MultiByteToWideChar(CP_UTF8, 0, base_path, -1, wdir, MAX_PATH);

    HANDLE dir_handle = CreateFileW(
        wdir,
        FILE_LIST_DIRECTORY,
        FILE_SHARE_WRITE | FILE_SHARE_READ | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS,
        NULL
    );

    if (dir_handle == INVALID_HANDLE_VALUE) {
        fprintf(stderr, "Failed to open directory handle\n");
        return;
    }

    char notify_buf[4096];
    DWORD bytes_returned;
    FILE_NOTIFY_INFORMATION* fni;
    unsigned long last = 0;
    int debounce_time = 4000;

    init(base_path); 

    while (1) {
        if (!ReadDirectoryChangesW(
            dir_handle,
            notify_buf,
            sizeof(notify_buf),
            TRUE,
            FILE_NOTIFY_CHANGE_LAST_WRITE | FILE_NOTIFY_CHANGE_FILE_NAME,
            &bytes_returned,
            NULL,
            NULL
        )) {
            fprintf(stderr, "Failed to read directory changes\n");
            break;
        }

        unsigned long current = now_ms();
        if ((current - last) < debounce_time) continue;

        fni = (FILE_NOTIFY_INFORMATION*)notify_buf;

        do {
            char filename[MAX_PATH];
            int count = WideCharToMultiByte(CP_UTF8, 0,
                fni->FileName, fni->FileNameLength / sizeof(WCHAR),
                filename, MAX_PATH - 1, NULL, NULL);
            filename[count] = '\0';

            if (fni->Action == FILE_ACTION_MODIFIED || fni->Action == FILE_ACTION_RENAMED_NEW_NAME) {
                printf("Modified: %s\n", filename);

                if (is_cfile(filename)) {
                    printf("\n=== C file modified ===\n");
                    process_cmake(base_path, 0);
                } else if (strstr(filename, "CMakeLists.txt")) {
                    printf("\n=== CMakeLists.txt modified ===\n");
                    process_cmake(base_path, 1);
                }

                const char* sofile = find_sofile(base_path);
                if (sofile) {
                    reloader(sofile);
                    free((void*)sofile);
                }

                last = current;
                Sleep(10);
            }

            if (!fni->NextEntryOffset) break;
            fni = (FILE_NOTIFY_INFORMATION*)((BYTE*)fni + fni->NextEntryOffset);
        } while (1);
    }

    CloseHandle(dir_handle);
}
