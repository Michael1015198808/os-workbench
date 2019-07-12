#include <devices.h>
#include <dir.h>
#include <vfs.h>
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


int block_read (device_t* dev,uint32_t off,uint32_t shift,char* s,size_t nbyte){
    size_t rest=nbyte;
    {
        uint32_t f_len=0x40-4-shift;
        dev->ops->read(dev,off+shift,s,f_len);
        s+=f_len;
        rest-=f_len;
        dev->ops->read(dev,off+0x40-4,&off,f_len);
    }
    for(;
            rest>0x40-4;
            rest-=0x40-4,s+=0x40-4){
        if(dev->ops->read(dev,off,s,0x40-4)<0||!off){
            return nbyte-rest;
        }
        dev->ops->read(dev,off+0x40-4,&off,4);
    }
    if(dev->ops->read(dev,off,s,rest)<0){
        return nbyte-rest;
    }
    return nbyte;
}

int block_write(device_t* dev,uint32_t off,uint32_t shift,const char* s,size_t nbyte){
    size_t rest=nbyte;
    {
        uint32_t f_len=0x40-4-shift;
        dev->ops->write(dev,off+shift,s,f_len);
        s+=f_len;
        rest-=f_len;
        dev->ops->read(dev,off+0x40-4,&off,f_len);
    }
    for(;
            rest>0x40-4;
            rest-=0x40-4,s+=0x40-4){
        if(dev->ops->write(dev,off,s,0x40-4)<0||!off){
            return nbyte-rest;
        }
        dev->ops->read(dev,off+0x40-4,&off,4);
        if(!off){
            TODO();
        }
    }
    if(dev->ops->write(dev,off,s,rest)<0){
        return nbyte-rest;
    }
    return nbyte;
}

uint32_t new_block(device_t* dev,uint32_t size){
    uint32_t ret;
    dev->ops->read(dev,0,&ret,4);
    uint32_t new_sz=ret+size;
    dev->ops->write(dev,0,&new_sz,4);
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

uint32_t find_block(device_t* dev,uint32_t off,uint64_t* fd_off){
    const uint32_t sz=0x40-4;//Size of information per block
    for(;*fd_off>sz;*fd_off-=sz){
        if(dev->ops->read(dev,off+sz,&off,4)!=4)return 0;
    }
    return off;
}
