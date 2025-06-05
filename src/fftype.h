#ifndef _FFTYPE_H
#define _FFTYPE_H

typedef struct {
    char** paths;
    int count;
} Paths;

Paths find_file_path(const char* base, const char* fType);

#endif