#include <klib.h>
#include <devices.h>
const char *warn="Haven't realized!\n";
int cat(void *args[],device_t *dev){
    dev->ops->write(dev,0,warn,strlen(warn));
    return 1;
}