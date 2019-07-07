#include <devices.h>
#include <yls.h>
#include <fs.h>

inodeops_t yls_iops;

static void yls_init(struct filesystem *fs,const char* name,device_t* dev){
    return;
}
inode_t *yls_lookup(struct filesystem* fs, const char* path, int flags){
    yls_node *cur=pmm->alloc(sizeof(yls_node));
#define read(off,buf,count) fs->dev->ops->read(fs->dev,off,buf,count);

    read(HEADER_LEN,cur,0x10);
    inode_t* ret=pmm->alloc(sizeof(inode_t));
    inode_t tmp={
        .ptr=cur,
        .fs=fs,
        .ops=&yls_iops
    };
    *ret=tmp;
    return ret;
}
int yls_close(inode_t *inode){
    return 0;
}

fsops_t yls_ops={
    .init=yls_init,
    .lookup=yls_lookup,
    .close=yls_close
};

inodeops_t yls_iops={
    /*
    .open   =yls_iopen,
    .close  =yls_iclose,
    .read   =yls_iread,
    .write  =yls_iwrite,
    .lseek  =yls_ilseek,
    .mkdir  =yls_imkdir,
    .rmdir  =yls_irmdir,
    .link   =yls_ilink,
    .unlink =yls_iunlink
    */
};
