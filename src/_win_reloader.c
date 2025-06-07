#include <stdio.h>
#include <windows.h>

int main() { 
    typedef void (*FuncType)();

    while (1) {
        printf("\n== Reloading ==\n");

        HMODULE dll = LoadLibraryA("./main.dll");
        if (dll == NULL) {
            printf("Failed loading dll file\n");
            return 1;
        }

        // get proc addr
        FuncType func = (FuncType)GetProcAddress(dll, "run_all");
        
        // call func
        if (func) func();

        // unload dll
        FreeLibrary(dll);

        printf("Please enter to reload\n");
        getchar();
    }

    return 0;
}