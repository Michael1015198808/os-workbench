#include <devices.h>
#include <dir.h>
#include <yls.h>
#include <fs.h>

//Compare filename and char*
int file_cmp(device_t* dev,uint32_t off,const char* s){
    dev->ops->read(dev,off+8,&off,4);
    //Transfer yls_node's offset into yls_node's name's offset
    return string_cmp(dev,off,s);
}

//Compare string blocks and char*
int string_cmp(device_t* dev,uint32_t off,const char* s){
    info string;
    int to_cmp;
    for(to_cmp=get_first_slash(s);
            to_cmp>0;
            to_cmp-=0x40-4,s+=0x40-4){
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
    Assert(0,"Should not reach here");
}
