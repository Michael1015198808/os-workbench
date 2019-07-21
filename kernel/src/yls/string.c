#include <devices.h>
#include <dir.h>
#include <vfs.h>
#include <yls.h>

//Compare filename and char*
int block_cmp(device_t* dev,uint32_t off,const char* s){
    char buf[0x80];
    ssize_t (*const read)(device_t* dev,off_t offset,void* buf,size_t count)=dev->ops->read;

    read(dev,off+4,&buf,80);
    int ret=strncmp(buf,s,BLK_SZ-8);
    if(ret)return ret;

    read(dev,off+BLK_MEM,&off,4);
    while(off){
        read(dev,off,buf,BLK_MEM);
        strncmp(buf,s,BLK_MEM);
        read(dev,off+BLK_MEM,&off,4);
    }
    return 0;
}

//Compare string blocks and char*
int string_cmp(device_t* dev,uint32_t off,const char* s){
    TODO();
}


int block_read (device_t* dev,uint32_t off,uint32_t shift,char* s,size_t nbyte){
    ssize_t (*const read)(device_t* dev,off_t offset,void* buf,size_t count)=dev->ops->read;
    size_t rest=nbyte;
    find_block(dev,&shift,&off);
    off+=shift;
    while(rest>0){
        int to_read=min(BLK_MEM-(off)%BLK_SZ,rest);
        if(read(dev,off,s,to_read)!=to_read){
            return nbyte-rest;
        }
        s+=to_read;
        rest-=to_read;
        if(read(dev,off+BLK_MEM,&off,4)!=4||!off){
            return nbyte-rest;
        };
    }
    return nbyte;
}

int block_write(device_t* dev,uint32_t off,uint32_t shift,const char* s,size_t nbyte){
    ssize_t (*const read)(device_t* dev,off_t offset,void* buf,size_t count)=dev->ops->read;
    ssize_t (*const write)(device_t* dev,off_t offset,const void* buf,size_t count)=dev->ops->write;
    size_t rest=nbyte;
    find_block(dev,&shift,&off);
    off+=shift;
    {
        int to_write=min(BLK_MEM-(off)%BLK_SZ,rest);
        if(write(dev,off,s,to_write)!=to_write){
            return nbyte-rest;
        }
        s   +=to_write;
        rest-=to_write;
        off +=to_write;
    }
    while(rest>0){
        uint32_t new_off=0;
        if(read(dev,off,&new_off,4)!=4||!new_off){
            new_off=new_block(dev);
            write(dev,off,&new_off,4);
            if(off==0){
                fprintf(2,"No more space on this disk!\n");
                return nbyte-rest;
            }
        }
        off=new_off;
        int to_write=min(BLK_MEM,rest);
        if(write(dev,off,s,to_write)!=to_write){
            return nbyte-rest;
        }
        s+=to_write;
        rest-=to_write;
        off+=to_write;
    }
    return nbyte;
}

uint32_t new_block(device_t* dev){
    uint8_t avail[0x40];
    dev->ops->read(dev,0,avail,0x40);
    for(int i=0;i<0x40;++i){
        if(avail[i]==0xff)continue;
        for(int j=0;j<8;++j){
            if(avail[i]&(1<<j)){
                continue;
            }
            uint32_t ret=(8*i+j)*0x80;
            avail[i]|=1<<j;
            dev->ops->write(dev,i,avail+i,1);
            return ret;
        }
    }
    return 0;
}

uint32_t new_inode(device_t* dev){
    uint32_t off;
    uint32_t buf;
    for(off=INODE_START;;off+=0x10){
        dev->ops->read(dev,off,&buf,0x4);
        if(buf==0){
            uint32_t one=1;
            dev->ops->write(dev,off,&one,0x4);
            return off;
        }
    }
}
//Find end of info
uint32_t find_end(device_t* dev,uint32_t off){
    uint32_t read;
    while(1){
        do{
            dev->ops->read(dev,off,&read,4);
            if(read==0){
                return off;
            }
            off+=4;
        }while((off%BLK_SZ)!=BLK_MEM);
        TODO();
        dev->ops->read(dev,off,&read,4);
        if(read==0){
            dev->ops->write(dev,off,&read,4);
            return read;
        }
    }
}

uint32_t find_empty(device_t* dev,uint32_t off){
    uint32_t read;
    while(1){
        do{
            dev->ops->read(dev,off,&read,4);
            if(read==0||read==YLS_WIPE){
                return off;
            }
            off+=4;
        }while((off%BLK_SZ)!=BLK_MEM);
        TODO();
        dev->ops->read(dev,off,&read,4);
        if(read==0){
            dev->ops->write(dev,off,&read,4);
            return read;
        }
    }
}

int find_block(device_t* dev,uint32_t* fd_off,uint32_t* off){
    for(;*fd_off>BLK_MEM;*fd_off-=BLK_MEM){
        if(dev->ops->read(dev,*off+BLK_MEM,off,4)!=4)return -1;
    }
    return 0;
}

uint32_t block_len(device_t* dev,uint32_t off){
    char buf[BLK_SZ];
    dev->ops->read(dev,off,buf,BLK_MEM-4);
    uint32_t ret=strnlen(buf,BLK_MEM-4);
    if(ret==BLK_MEM-4){
        TODO();
    }
    return ret;
}
