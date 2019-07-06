#include <klib.h>
#include <devices.h>

int echo(void *args[],device_t *dev){
    int i=1;
    while(1){
        vfs->write(0,info(args[i]));
        ++i;
        if(args[i]){
            vfs->write(0,info(" "));
        }else{
            vfs->write(0,info("\n"));
            return 0;
        }
    }
}
