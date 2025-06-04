#include "reloc.h"

#include <stdlib.h>
#include <string.h>

void append_string(char*** arr, int* size, const char* str) {
    char **new_arr = realloc(*arr, (*size + 1) * sizeof(char*));
    if (!new_arr) return;
    *arr = new_arr;

    char *new_str = strdup(str);
    if (!new_str) return;  

    (*arr)[*size] = new_str;
    (*size)++;    
}


