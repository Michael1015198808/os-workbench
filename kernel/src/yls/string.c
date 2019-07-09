#include <devices.h>
#include <yls.h>
#include <fs.h>

//Compare filename and char*
int file_cmp(device_t* dev,uint32_t off,char* s){
    dev->ops->read(dev,off+8,&off,4);
    return string_cmp(dev)
}

//Compare string blocks and char*
int string_cmp(device_t* dev,uint32_t off,char* s){
    info string;
    for(int to_cmp=get_first_slash(s);to_cmp>0;to_cmp-=0x40-4){
        dev->ops->read(dev,off,&string,sizeof(info));
        if(to_cmp>0x40-4){
            int ret=strncmp(string.mem,s,0x40-4);
            if(ret){
                return ret;
            }else{
                dev->ops->read(dev,string.next,&string,sizeof(info));
            }
        }else{
            return strncmp(string.mem,s,to_cmp);
        }
    }
}
