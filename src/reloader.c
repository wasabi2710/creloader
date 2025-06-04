#include <stdio.h>
#include <dlfcn.h>

int main() {
    void* handle;
    void (*hello_func)();

    while (1) {
        printf("\n=== Reloading hello.so ===\n");

        // Load the library
        handle = dlopen("./hello.so", RTLD_NOW);
        if (!handle) {
            fprintf(stderr, "Failed to load: %s\n", dlerror());
            return 1;
        }

        // Find the symbol
        hello_func = (void (*)())dlsym(handle, "hello");
        if (!hello_func) {
            fprintf(stderr, "Failed to find hello(): %s\n", dlerror());
            dlclose(handle);
            return 1;
        }

        // Call the function
        hello_func();

        // Close the library
        dlclose(handle);

        // Wait before reloading
        printf("Press Enter to reload, Ctrl+C to exit...\n");
        getchar();
    }

    return 0;
}
