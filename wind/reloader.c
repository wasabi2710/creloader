#include "reloader.h"

void reloader(const char* dllfile) {
    printf("\n=== Reloading ===\n");

    HMODULE lib = LoadLibraryA(dllfile);
    if (!lib) {
        fprintf(stderr, "RELOADER: failed LoadLibrary: %lu\n", GetLastError());
        return;
    }

    void (*run_all)() = (void (*)())GetProcAddress(lib, REDEFENTRY);
    if (!run_all) {
        fprintf(stderr, "RELOADER: failed GetProcAddress for %s\n", REDEFENTRY);
        FreeLibrary(lib);
        return;
    }

    run_all();
    FreeLibrary(lib);
}
