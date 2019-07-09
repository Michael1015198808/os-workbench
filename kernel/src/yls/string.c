#include <devices.h>
#include <dir.h>
#include <fs.h>
#include <yls.h>

//Compare filename and char*
int file_cmp(device_t* dev,uint32_t off,const char* s){
    dev->ops->read(dev,off+8,&off,4);
    //Transfer yls_node's offset into yls_node's name's offset
    return string_cmp(dev,off,s);
}

//Compare string blocks and char*
int string_cmp(device_t* dev,uint32_t off,const char* s){
    info string;
    int to_cmp=get_first_layer(s);
    for(;
            to_cmp>0;
            to_cmp-=0x40-4,s+=0x40-4){
        dev->ops->read(dev,off,&string,sizeof(info));
        if(to_cmp>0x40-4){
            int ret=strncmp(string.mem,s,0x40-4);
            if(ret){
                return ret;
            }else if(string.next){
                dev->ops->read(dev,string.next,&string,sizeof(info));
            }else{
                return 1;
            }
        }else{
            return strncmp(string.mem,s,to_cmp);
        }
    }
    Assert(0,"Should not reach here");
}

int string_cpy(device_t* dev,uint32_t off,const char* s){
    int to_cpy=strlen(s),ret=to_cpy;
    for(;
            to_cpy>0;
            to_cpy-=0x40-4,s+=0x40-4){
        if(dev->ops->write(dev,off,s,0x40-4)<0){
            Assert(0,"Should not reach here!");
        }
        uint32_t new_off=off+0x40;
        dev->ops->write(dev,off+0x40-4,&new_off,4);
    }
    return ret;
}

uint32_t new_block(device_t* dev,uint32_t size){
    uint32_t ret;
    dev->ops->read(dev,0,&ret,4);
    return ret;
}
//Find end of info
uint32_t find_end(device_t* dev,uint32_t off){
    uint32_t read;
    while(1){
        for(int i=0;i<OFFS_PER_MEM;++i,off+=4){
            dev->ops->read(dev,off,&read,4);
            if(read==0){
                return off;
            }
        }
        dev->ops->read(dev,off,&read,4);
        if(read==0){
            read=new_block(dev,0x40);
            dev->ops->write(dev,off,&read,4);
            return read;
        }
    }
}
