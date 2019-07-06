#include <klib.h>
#include <devices.h>
const char *warn="Haven't realized!\n";
int cat(void *args[],device_t *dev){
    char buf[0x200];
    int nread=0;
    while( (nread=
                vfs->read(STDIN,buf,sizeof(buf))) ){
        if(buf[nread]=='\0')break;
        buf[nread]='\0';
        std_write(buf);
    }
    return 1;
}
