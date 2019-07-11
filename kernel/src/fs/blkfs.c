#include <devices.h>
#include <vfs.h>
#include <common.h>

static void blkfs_init(filesystem* fs,const char* name,device_t* dev){
    fs->name=name;
    fs->ops=NULL;
    fs->dev=NULL;

    fs->inodeops=&blkfs_iops;
}

static fsops_t blkfs_ops={
    .init  =blkfs_init,
    /*
    .lookup=blkfs_lookup,
    .close =blkfs_close,
    */
};

static inodeops_t blkfs_iops={
    /*
    .open  =blkfs_iopen,
    .close =blkfs_iclose,
    .read  =blkfs_iread,
    .write =blkfs_iwrite,
    */
};

filesystem blkfs[2]={
    {
        .ops     =&blkfs_ops,
        .inodeops=&blkfs_iops,
    },
    {
        .ops     =&blkfs_ops,
        .inodeops=&blkfs_iops,
    },
};
