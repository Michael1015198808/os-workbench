#include <klib.h>
#include <devices.h>
int echo(void *args[],device_t *dev){
    int i=1;
    while(1){
        dev->ops->write(dev,0,(char*)(args[i]),strlen(args[i]));
        ++i;
        if(args[i]){
            dev->ops->write(dev,0, " ",1);
        }else{
            dev->ops->write(dev,0,"\n",1);
            return 0;
        }
    }
}
