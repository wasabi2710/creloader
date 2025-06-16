#include <windows.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <wchar.h>
#include "watcher.h"

bool is_c_file(const wchar_t* path) {
    const wchar_t* ext = wcsrchr(path, L'.');
    return ext && (wcscmp(ext, L".c") == 0 || wcscmp(ext, L".h") == 0);
}

bool is_temp_file(const wchar_t* filename) {
    return (iswdigit(filename[0]) || filename[0] == L'~');
}

void watch_src_dir(const wchar_t* dirpath, FuncPtr func[], void* args[], int c) {
    HANDLE hDir = CreateFileW(
        dirpath,
        FILE_LIST_DIRECTORY,
        FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
        NULL,
        OPEN_EXISTING,
        FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OVERLAPPED,
        NULL
    );

    if (hDir == INVALID_HANDLE_VALUE) {
        wprintf(L"Error: Cannot open '%s' (Code %d)\n", dirpath, GetLastError());
        return;
    }

    DWORD buf_size = INITIAL_BUFFER_SIZE;
    BYTE* buffer = (BYTE*)malloc(buf_size);
    if (!buffer) {
        wprintf(L"Error: Failed to allocate initial buffer\n");
        CloseHandle(hDir);
        return;
    }

    OVERLAPPED overlapped = {0};
    overlapped.hEvent = CreateEvent(NULL, TRUE, FALSE, NULL);
    if (!overlapped.hEvent) {
        wprintf(L"Error: Failed to create event (Code %d)\n", GetLastError());
        free(buffer);
        CloseHandle(hDir);
        return;
    }

    ULONGLONG last_modified_time = 0;
    wchar_t last_modified_file[MAX_PATH] = L"";

    while (1) {
        DWORD bytesReturned = 0;

        if (!ReadDirectoryChangesW(
            hDir,
            buffer,
            buf_size,
            TRUE,  // Monitor subdirs
            FILE_NOTIFY_CHANGE_FILE_NAME | FILE_NOTIFY_CHANGE_LAST_WRITE,
            &bytesReturned,
            &overlapped,
            NULL
        )) {
            wprintf(L"ReadDirectoryChangesW failed (Error %d)\n", GetLastError());
            break;
        }

        DWORD wait_status = WaitForSingleObject(overlapped.hEvent, INFINITE);
        if (wait_status != WAIT_OBJECT_0) {
            wprintf(L"Wait failed (Status %d)\n", wait_status);
            break;
        }

        if (!GetOverlappedResult(hDir, &overlapped, &bytesReturned, FALSE)) {
            DWORD err = GetLastError();
            if (err == ERROR_NOTIFY_ENUM_DIR) {
                wprintf(L"Buffer overflow detected — increasing buffer size\n");
                if (buf_size * 2 <= MAX_BUFFER_SIZE) {
                    free(buffer);
                    buf_size *= 2;
                    buffer = (BYTE*)malloc(buf_size);
                    if (!buffer) {
                        wprintf(L"Error: Failed to allocate buffer of size %lu KB\n", buf_size / 1024);
                        break;
                    }
                    wprintf(L"Buffer increased to %lu KB\n", buf_size / 1024);
                    ResetEvent(overlapped.hEvent);
                    continue;  // Retry with bigger buffer
                } else {
                    wprintf(L"Max buffer size reached!\n");
                    break;
                }
            } else {
                wprintf(L"Overlapped error (Error %d)\n", err);
                break;
            }
        }

        if (bytesReturned == 0) {
            wprintf(L"No data received — possible overflow or no events\n");
            ResetEvent(overlapped.hEvent);
            continue;
        }

        // --- Process Events ---
        FILE_NOTIFY_INFORMATION* event = (FILE_NOTIFY_INFORMATION*)buffer;
        while (1) {
            wchar_t filepath[MAX_PATH];
            wcscpy(filepath, dirpath);
            wcscat(filepath, L"\\");
            wcsncat(filepath, event->FileName, event->FileNameLength / sizeof(wchar_t));
            filepath[MAX_PATH - 1] = L'\0';  // Safety null terminate

            if (!is_temp_file(event->FileName) && is_c_file(filepath)) {
                ULONGLONG current_time = GetTickCount64();
                if (event->Action == FILE_ACTION_MODIFIED) {
                    if (wcscmp(last_modified_file, filepath) == 0 &&
                        (current_time - last_modified_time) < DEBOUNCE_MS) {
                        goto next_event;  // please skip duplicate
                    }
                    last_modified_time = current_time;
                    wcscpy(last_modified_file, filepath);
                }

                const wchar_t* action = NULL;
                switch (event->Action) {
                    case FILE_ACTION_ADDED: action = L"ADDED"; break;
                    case FILE_ACTION_MODIFIED: 
                        printf("modified\n");
                        action = L"MODIFIED"; 
                        // recall any funcptr[]
                        for (int i = 0; i < c; i++) {
                            func[i](args[i]);
                        }
                        break;
                    case FILE_ACTION_REMOVED: action = L"REMOVED"; break;
                    case FILE_ACTION_RENAMED_OLD_NAME: action = L"RENAMED_FROM"; break;
                    case FILE_ACTION_RENAMED_NEW_NAME: action = L"RENAMED_TO"; break;
                    default: action = L"UNKNOWN"; break;
                }
                wprintf(L"[%s] %s\n", action, filepath);
            }

        next_event:
            if (event->NextEntryOffset == 0)
                break;
            event = (FILE_NOTIFY_INFORMATION*)((BYTE*)event + event->NextEntryOffset);
        }

        ResetEvent(overlapped.hEvent);
    }

    free(buffer);
    CloseHandle(overlapped.hEvent);
    CloseHandle(hDir);
}


