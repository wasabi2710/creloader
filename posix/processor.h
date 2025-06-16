#ifndef _PROCESSOR_H
#define _PROCESSOR_H

#include <dirent.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <libgen.h>
#include <syscall.h>
#include <errno.h>
#include <time.h>

void process_cmake(const char* src_dir, int rebuild);
char* find_sofile(const char* src_dir);

#endif
