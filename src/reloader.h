#ifndef _RELOADER_H
#define _RELOADER_H

typedef struct {
    const char* func_name;
    void (*func)(void);
} FuncType;

void _reloader(const char* dll_path, FuncType* func_type);
void _init_reloader(const char* src_path, char* new_path);

#endif