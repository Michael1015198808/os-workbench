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

static inode_t devfs_root;//definition is at the end

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
    if((!path[0])||(path[0]=='\\'&&path[1]=='\0')){
        return &devfs_root;
    }
    ++path;
    for (int i = 0; i < devices_cnt; i++) 
        if (strcmp(devices[i]->name, path) == 0)
            return fs->inodes+i;
    fprintf(2,"%s: No such a file or directory",path);
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
    file->offset=0;
    file->flags=flags;
    file->refcnt=1;
    file->lk=PTHREAD_MUTEX_INITIALIZER;
    return 0;
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

static ssize_t devfs_ireaddir(vfile_t* file,char* buf,size_t size){
    if(file->inode!=&devfs_root){
        warn("%s: Not a directory",get_dev(file)->name);
    }
    if(file->offset==devices_cnt)return 0;
    strcpy(buf,devices[file->offset]->name);
    ++file->offset;
    ssize_t nread=strlen(buf);
    return nread;
}

static ssize_t devfs_iwrite(vfile_t* file,const char* buf,size_t size){
    device_t* dev=get_dev(file);
    ssize_t ret  =dev->ops->write(dev,file->offset,buf,size);
    file->offset+=ret;
    return ret;
}

static ssize_t devfs_ilseek(vfile_t* file,off_t offset,int whence){
    switch(whence){
        case SEEK_SET:
            return file->offset=offset;
        case SEEK_CUR:
            return file->offset+=offset;
        case SEEK_END:
            TODO();
    }
    BARRIER();
}

static ssize_t devfs_imkdir(const char* name){
    fprintf(STDERR,"cannot create directory ‘%s’: Read-only filesystem\n",name);
    return -1;
}

static ssize_t devfs_irmdir(const char* name){
    fprintf(STDERR,"cannot remove directory ‘%s’: Read-only filesystem\n",name);
    return -1;
}

static ssize_t devfs_ilink(const char* name, inode_t* inode){
    fprintf(STDERR,"cannot create file ‘%s’: Read-only filesystem\n",name);
    return -1;
}

static ssize_t devfs_iunlink(const char* name){
    fprintf(STDERR,"cannot remove file ‘%s’: Read-only filesystem\n",name);
    return -1;
}

//.func_name=dev_ifunc_name
//i for inode
static inodeops_t devfs_iops={
    .open   =devfs_iopen,
    .close  =devfs_iclose,
    .read   =devfs_iread,
    .readdir=devfs_ireaddir,
    .write  =devfs_iwrite,
    .lseek  =devfs_ilseek,
    .mkdir  =devfs_imkdir,
    .rmdir  =devfs_irmdir,
    .link   =devfs_ilink,
    .unlink =devfs_iunlink,
};

filesystem devfs={
    .ops=&devfs_ops,
    .dev=NULL,
    .inodeops=&devfs_iops,
};

static inode_t devfs_root={
    .ptr=NULL,
    .fs =&devfs,
    .ops=&devfs_iops,
};
