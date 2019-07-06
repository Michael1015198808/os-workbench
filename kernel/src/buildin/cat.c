#include <klib.h>
#include <devices.h>
const char *warn="Haven't realized!\n";
int cat(void *args[],device_t *dev){
    char buf[0x200];
    vfs->read(STDIN,0,buf,sizeof(buf));
    std_write(buf);
    return 1;
}
