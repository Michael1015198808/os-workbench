#include <klib.h>
int echo(void *args[],device_t *dev){
    int i=1;
    while(1){
        dev->ops->write("%s",(char*)(args[i]));
        ++i;
        if(args[i]){
            dev->ops->write(dev,0, " ",2);
        }else{
            dev->ops->write(dev,0,"\n",2);
            return 0;
        }
    }
}
