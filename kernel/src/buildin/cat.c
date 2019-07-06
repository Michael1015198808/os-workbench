#include <klib.h>
#include <devices.h>
const char *warn="Haven't realized!\n";
int cat(void *args[],device_t *dev){
    char buf[0x200];
    while(1){
        int nread=vfs->read(STDIN,buf,sizeof(buf));
        buf[nread]='\0';
        std_write(buf);
    }
    return 1;
}
