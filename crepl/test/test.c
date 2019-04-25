#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dlfcn.h>

int main(int argc, char** argv)
{
    void *handle;
    int (*func_print_name)(void);

    //handle = dlopen("./libdog.so", RTLD_NOW|RTLD_GLOBAL);
    handle = dlopen("./test.so", RTLD_NOW|RTLD_GLOBAL);
    if (!handle) {
        /* fail to load the library */
        fprintf(stderr, "Error: %s\n", dlerror());
        return EXIT_FAILURE;
    }
    *(void**)(&func_print_name) = dlsym(handle, "fun");
    if (!func_print_name) {
        /* no such symbol */
        fprintf(stderr, "Error: %s\n", dlerror());
        dlclose(handle);
        return EXIT_FAILURE;
    }
    printf("Return value:%d\n",func_print_name());
    dlclose(handle);
    return EXIT_SUCCESS;
}
