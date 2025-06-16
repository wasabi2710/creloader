#ifndef _WATCHER_H
#define _WATCHER_H

#define INITIAL_BUFFER_SIZE 65536  // 64KB
#define MAX_BUFFER_SIZE     (10 * 1024 * 1024)  
#define DEBOUNCE_MS         200      

typedef void (*FuncPtr)(void*);

void watch_src_dir(const wchar_t* dirpath, FuncPtr func[], void* args[], int c);

#endif