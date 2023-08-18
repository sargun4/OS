#include <stdio.h>
#include <stdlib.h>
#include <dlfcn.h>

int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <executable>\n", argv[0]);
        return 1;
    }

    // Load the shared library
    void *lib_handle = dlopen("path/to/lib_simpleloader.so", RTLD_LAZY);
    if (!lib_handle) {
        fprintf(stderr, "Error loading library: %s\n", dlerror());
        return 1;
    }

    // Get function pointers to load_and_execute and cleanup
    void (*load_and_execute)(const char *) = dlsym(lib_handle, "load_and_execute");
    void (*cleanup)() = dlsym(lib_handle, "cleanup");

    // Execute the loader
    load_and_execute(argv[1]);

    // Clean up resources
    cleanup();
    dlclose(lib_handle);

    return 0;
}
