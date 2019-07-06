#include <klib.h>
#include <devices.h>

int echo(void *args[],device_t *dev){
    int i=1;
    while(1){
        std_write(0,winfo(args[i]));
        ++i;
        if(args[i]){
            std_write(0,winfo(" "));
        }else{
            std_write(0,winfo("\n"));
            return 0;
        }
    }
}
