#ifndef _RELOADER_H
#define _RELOADER_H

#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

#define REDEFENTRY "run_all"

void reloader(const char* sofile);

#endif
