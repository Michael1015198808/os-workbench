#include <devices.h>
#include <dir.h>
#include <vfs.h>
#include <yls.h>

//Compare filename and char*
int file_cmp(device_t* dev,uint32_t off,const char* s){
    char buf[0x80];
    dev->ops->read(dev,off+4,&buf,80);
    int ret=strncmp(buf,s,0x80-8);
    if(ret)return ret;
    TODO();//File name longer than 0x78
    return string_cmp(dev,off,s);
}

//Compare string blocks and char*
int string_cmp(device_t* dev,uint32_t off,const char* s){
    TODO();
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
    ssize_t (*const read)(device_t* dev,off_t offset,void* buf,size_t count)=dev->ops->read;
    size_t rest=nbyte;
    while(shift>=BLK_MEM){
        shift-=BLK_MEM;
        read(dev,off+BLK_MEM,&off,4);
    }
    off+=shift;
    while(rest>0){
        int to_read=off^BLK_SZ;
        if(dev->ops->read(dev,off,s,to_read)<0||!off){
            return nbyte-rest;
        }
        s+=to_read;
        dev->ops->read(dev,off+BLK_MEM,&off,4);
        rest-=to_read;
    }
    return nbyte;
}

int block_write(device_t* dev,uint32_t off,uint32_t shift,const char* s,size_t nbyte){
    TODO();
}

uint32_t new_block(device_t* dev,uint32_t size){
    uint8_t avail[0x40]
    dev->ops->read(dev,0,avail,0x40);
    for(int i=0;i<0x40;++i){
        for(int j=0;j<8;++j){
            if(avail[i]&(1<<j)){
                return (8*i+j)*0x80;
            }
        }
    }
    return 0;
}
//Find end of info
uint32_t find_end(device_t* dev,uint32_t off){
    TODO();
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
    TODO();
    const uint32_t sz=0x40-4;//Size of information per block
    for(;*fd_off>sz;*fd_off-=sz){
        if(dev->ops->read(dev,off+sz,&off,4)!=4)return 0;
    }
    return off;
}
