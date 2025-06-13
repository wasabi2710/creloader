#include "reloader.h"
#include <dlfcn.h>

void reloader(const char* sofile) {
    printf("\n===== Reloading =====\n");

    void* genhand;
    genhand = dlopen(sofile, RTLD_NOW);
    if (!genhand) {
        fprintf(stderr, "RELOADER: failed dlopen sofile: %s\n", dlerror());
        return;
    }

    void (*run_all)() = dlsym(genhand, REDEFENTRY);
    if (!run_all) {
        fprintf(stderr, "RELOADER: failed dlsym main entry\n");
        return;
    }  

    run_all();

    dlclose(genhand);
}