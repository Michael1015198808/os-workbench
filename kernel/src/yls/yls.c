#include <devices.h>
#include <common.h>
#include <yls.h>
#include <vfs.h>
#include <dir.h>
static inline ssize_t write(device_t* dev,off_t offset,uint32_t to_write,size_t count){
    uint32_t tmp=to_write;
    dev->ops->write(dev,offset,&tmp,4);
}

int yls_init(device_t* dev){

    write(dev,0,0xff,4);

    yls_node root_inode={
        .refcnt=1,
        .info=0x480,
        .size=8,
        .type=YLS_DIR,
    };
    block root_name{
        .mem="/",
        .next=0,
    };
    dev->ops->write(dev,0x040,&root_inode,sizeof(root_inode));
    dev->ops->write(dev,0x400,&root_name,sizeof(root_name));

    return 0;
}
