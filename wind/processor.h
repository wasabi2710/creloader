#ifndef _PROCESSOR_H
#define _PROCESSOR_H

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void process_cmake(const char* src_dir, int rebuild);
char* find_sofile(const char* src_dir);

#endif