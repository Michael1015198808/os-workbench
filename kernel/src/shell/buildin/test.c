#include <klib.h>
#include <devices.h>

int test(void *args[]){
    open("./tags",O_RDWR);
    return 0;
}
