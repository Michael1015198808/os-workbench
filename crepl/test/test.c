#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

int main(int argc, char** argv)
{
    void *handle;
    void (*func_print_name)(const char*);

    handle = dlopen("./libdog.so", RTLD_LAZY|RTLD_GLOBAL);
    handle = dlopen("./libcat.so", RTLD_LAZY);
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
    func_print_name("cat");
    dlclose(handle);
    return EXIT_SUCCESS;
}
