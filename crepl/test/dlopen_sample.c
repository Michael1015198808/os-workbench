#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

int main(int argc, char** argv)
{
    void *handle;
    void (*func_print_name)(const char*);

    if (argc != 2) {
        fprintf(stderr, "Usage: %s animal_type\n", argv[0]);
        return EXIT_FAILURE;
    }

    if (strcmp(argv[1], "dog") == 0) {
        handle = dlopen("./libdog.so", RTLD_LAZY);
    } else if (strcmp(argv[1], "cat") == 0) {
        handle = dlopen("./libcat.so", RTLD_LAZY);
    } else {
        fprintf(stderr, "Error: unknown animal type: %s\n", argv[1]);
        return EXIT_FAILURE;
    }
    if (!handle) {
        /* fail to load the library */
        fprintf(stderr, "Error: %s\n", dlerror());
        return EXIT_FAILURE;
    }

    *(void**)(&func_print_name) = dlsym(handle, "print_name");
    if (!func_print_name) {
        /* no such symbol */
        fprintf(stderr, "Error: %s\n", dlerror());
        dlclose(handle);
        return EXIT_FAILURE;
    }

    func_print_name(argv[1]);
    dlclose(handle);

    return EXIT_SUCCESS;
}
