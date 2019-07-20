#include <devices.h>
#include <common.h>
#include <vfs.h>
#include <fs.h>
/*
 * devfs_init
 * devfs_lookup
 * devfs_close
 */

extern const device_t *devices[];
extern const size_t devices_cnt;


static inodeops_t devfs_iops;
static void devfs_init(filesystem* fs,const char* name,device_t *dev){
    fs->name  =name;
    fs->dev   =dev;
    fs->inodes=pmm->alloc(sizeof(inode_t)*devices_cnt);

    fs->root  =pmm->alloc(sizeof(inode_t));
    *fs->root =(inode_t){
            .ptr=NULL,
            .fs =fs,
            .ops=&devfs_iops,
        };
    fs->root->ptr=dev_lookup("null");
    for(int i=0;i<devices_cnt;++i){
        fs->inodes[i].ptr=(void*)devices[i];
        fs->inodes[i].fs=fs;
        fs->inodes[i].ops=fs->inodeops;
    }
}

static inode_t* devfs_lookup(filesystem* fs,const char* path,int flags){
    if((!path[0]))return devfs.root;
    return vfs_find(devfs.root,path,flags);
}

static int devfs_close(inode_t* inode){
    return 0;
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
    pthread_mutex_lock(&file->lk);
    --file->refcnt;
    if(file->refcnt==0){
        return file->inode->fs->ops->close(file->inode);
    }
    pthread_mutex_unlock(&file->lk);
    return 0;
}

static ssize_t devfs_iread(vfile_t* file,char* buf,size_t size){
    device_t* dev=get_dev(file);
    ssize_t ret;
    if(dev){
        ret = dev->ops->read(dev,file->offset,buf,size);
        file->offset+=ret;
    }else{
        ret = EISDIR;
    }
    return ret;
}

static ssize_t devfs_ireaddir(vfile_t* file,char* buf,size_t size){
    if(file->inode!=devfs.root){
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

static int is_dir(inode_t* cur){
    return cur==devfs.root;
}

static inode_t* devfs_ifind(inode_t* cur,const char* path,int flags){
    inode_t* next=NULL;
    check(cur,path,flags);

    if(cur==devfs.root){
        if(path[0]=='.'){
            if(path[1]=='.'){
                //.. for parent
                next=devfs.root_parent;
                path+=2;
            }else{
                //. for current
                next=cur;
                ++path;
            }
        }else{
            for(int i=0;i<devices_cnt;++i){
                if(strcmp(devices[i]->name, path)==0){
                    next=devfs.inodes+i;
                    path+=strlen(devices[i]->name);
                }
            }
        }
    }
    return vfs_find(next,path,flags);
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
    .find   =devfs_ifind,
};

filesystem devfs={
    .ops     =&devfs_ops,
    .dev     =NULL,
    .inodeops=&devfs_iops,
};


int isatty(int fd){
    task_t* cur=get_cur();
    device_t*  dev=get_dev(cur->fd[fd]);
    extern devops_t tty_ops;
    return dev->ops==&tty_ops;
}
