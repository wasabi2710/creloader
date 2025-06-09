#ifndef _WATCHER_H
#define _WATCHER_H

#include <sys/inotify.h>
#include <stdio.h>  

void mon_dir(const char* src_dir);
void src_watcher(const char* base_path);

#endif
