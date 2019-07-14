#include <devices.h>
#include <vfs.h>
#include <common.h>
/*
 * devfs_init
 * devfs_lookup
 * devfs_close
 */

extern device_t *devices[];
extern size_t devices_cnt;

static inodeops_t devfs_iops;
static void devfs_init(filesystem* fs,const char* name,device_t *dev){
    fs->name=name;
    fs->dev=dev;
    fs->inodes=pmm->alloc(sizeof(inode_t)*devices_cnt);

    for(int i=0;i<devices_cnt;++i){
        fs->inodes[i].ptr=devices[i];
        fs->inodes[i].fs=fs;
        fs->inodes[i].ops=fs->inodeops;
    }
}

static inode_t* devfs_lookup(filesystem* fs,const char* path,int flags){
    for (int i = 0; i < devices_cnt; i++) 
        if (strcmp(devices[i]->name, name) == 0)
            return fs->inodes[i];
    panic("lookup device failed.");
    return NULL;
}

static int devfs_close(inode_t* inode){
    TODO();
}

static fsops_t devfs_ops={
    .init  =devfs_init,
    .lookup=devfs_lookup,
    .close =devfs_close,
};
/*
 * devfs_iopen
 * devfs_iclose
 * devfs_iread
 * devfs_iwrite
 * devfs_ilseek
 * devfs_imkdir
 * devfs_irmdir
 * devfs_ilink
 * devfs_unlink
 */
static device_t* get_dev(vfile_t* file){
    return (device_t*)file->inode->ptr;
}

static int devfs_iopen(vfile_t* file,int flags){
    TODO();
}

static int devfs_iclose(vfile_t* file){
    TODO();
}

static ssize_t devfs_iread(vfile_t* file,char* buf,size_t size){
    device_t* dev=get_dev(file);
    ssize_t ret  =dev->ops->read(dev,file->offset,buf,size);
    file->offset+=ret;
    return ret;
}

static ssize_t devfs_iwrite(vfile_t* file,const char* buf,size_t size){
    device_t* dev=get_dev(file);
    ssize_t ret  =dev->ops->write(dev,file->offset,buf,size);
    file->offset+=ret;
    return ret;
}
//.func_name=dev_ifunc_name
//i for inode
static inodeops_t devfs_iops={
    .open  =devfs_iopen,
    .close =devfs_iclose,
    .read  =devfs_iread,
    .write =devfs_iwrite,
};

filesystem devfs={
    .ops=&devfs_ops,
    .dev=NULL,
    .inodeops=&devfs_iops,
};
