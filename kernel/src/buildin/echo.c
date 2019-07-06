#include <klib.h>
#include <devices.h>

int echo(void *args[],device_t *dev){
    int i=1;
    while(1){
        std_write(winfo(args[i]));
        ++i;
        if(args[i]){
            std_write(winfo(" "));
        }else{
            std_write(winfo("\n"));
            return 0;
        }
    }
}
