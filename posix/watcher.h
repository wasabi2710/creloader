#ifndef _WATCHER_H
#define _WATCHER_H

#include <sys/inotify.h>
#include <linux/limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
#include <limits.h>
#include <sys/stat.h>
#include <libgen.h>
#include <sys/time.h>

#include "reloader.h"
#include "processor.h"

void mon_dir(const char* src_dir);
void src_watcher(const char* base_path);

#endif
