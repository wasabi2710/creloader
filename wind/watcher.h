#ifndef _WATCHER_H
#define _WATCHER_H

#include <windows.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/timeb.h>

#define MAX_EVENTS 1024

void watch_directory(const char* base_path);

#endif