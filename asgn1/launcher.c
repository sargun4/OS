// #include <stdio.h>
// #include <stdlib.h>
// #include <dlfcn.h>

// int main(int argc, char *argv[]) {
//     if (argc != 2) {
//         fprintf(stderr, "Usage: %s <executable>\n", argv[0]);
//         return 1;
//     }

//     // Load the shared library
//     void *lib_handle = dlopen("path/to/lib_simpleloader.so", RTLD_LAZY);
//     if (!lib_handle) {
//         fprintf(stderr, "Error loading library: %s\n", dlerror());
//         return 1;
//     }

//     // Get function pointers to load_and_execute and cleanup
//     void (*load_and_execute)(const char *) = dlsym(lib_handle, "load_and_execute");
//     void (*cleanup)() = dlsym(lib_handle, "cleanup");

//     // Execute the loader
//     load_and_execute(argv[1]);

//     // Clean up resources
//     cleanup();
//     dlclose(lib_handle);

//     return 0;
// }
#include <stdio.h>
#include <dlfcn.h> // For dynamic loading of shared libraries
#include "loader.h" // Include the header with loader APIs
int main(int argc, char *argv[]) {
    if (argc != 2) {
        fprintf(stderr, "Usage: %s <elf_file>\n", argv[0]);
        return 1;
    }
    // Load the loader library
    void *loader_handle = dlopen("./bin/lib_simpleloader.so", RTLD_LAZY);
    if (!loader_handle) {
        fprintf(stderr, "Error loading lib_simpleloader.so: %s\n", dlerror());
        return 1;
    }
    // Get function pointers to the loader APIs
    LoadAndExecuteFn load_and_execute = dlsym(loader_handle, "load_and_execute");
    CleanupFn cleanup = dlsym(loader_handle, "cleanup");
    if (!load_and_execute || !cleanup) {
        fprintf(stderr, "Error getting function pointers from lib_simpleloader.so\n");
        dlclose(loader_handle);
        return 1;
    }
    // Load and execute the ELF file using the loader APIs
    load_and_execute(argv[1]);
    // Cleanup after execution
    cleanup();
    // Close the loader library
    dlclose(loader_handle);
    return 0;
}
