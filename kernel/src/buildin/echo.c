#include <klib.h>
#include <devices.h>

int echo(void *args[],device_t *dev){
    int i=1;
    while(1){
        vfs->write(0,winfo(args[i]));
        ++i;
        if(args[i]){
            vfs->write(0,winfo(" "));
        }else{
            vfs->write(0,winfo("\n"));
            return 0;
        }
    }
}
