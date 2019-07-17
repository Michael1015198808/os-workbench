#include <devices.h>
#include <vfs.h>
#include <common.h>
/*
 * procfs_init
 * procfs_lookup
 * dprocs_close
 */

static inode_t procfs_root;
extern task_t *tasks[0x40];

#define PROC_DIR  0
#define PROC_PWD  1
#define PROC_NAME 2
const char* proc_info[]={
    NULL,
    "pwd",
    "name",
};

static void procfs_init(filesystem* fs,const char* name,device_t *dev){
    fs->name=name;
    fs->dev=dev;
    fs->inodes=pmm->alloc(sizeof(inode_t)*0x40*3);

    for(int i=0;i<0x40;++i){
        for(int j=0;j<3;++j){
            int idx=i*3+j;
            fs->inodes[idx].ptr=pmm->alloc(sizeof(uint8_t)*2);
            uint8_t *p=fs->inodes[idx].ptr;
            p[0]=i;p[1]=j;
            fs->inodes[idx].fs=fs;
            fs->inodes[idx].ops=fs->inodeops;
        }
    }
}

static inode_t* procfs_lookup(filesystem* fs,const char* path,int flags){
    if((!path[0])||(path[0]=='/'&&path[1]=='\0')){
        if(flags&O_WRONLY){
            warn("%s%s: Is a dictionary",fs->mount,path);
            return NULL;
        }else{
            return &procfs_root;
        }
    }
    int i;
    char num[3];
    for(i=0;i<3;++i){
        if(*path>='0'&&*path<='9'){
            num[i]=*path;
            ++path;
        }else{
            num[i]='\0';
            break;
        }
    }
    if(i<3){
        int id=atoi(num);
        if(tasks[id]){
            id*=3;
            if(path){
                for(int i=0;i<3;++i){
                    if(!strcmp(path,proc_info[i])){
                        return procfs.inodes+id+i;
                    }
                }
            }else{
                return procfs.inodes+id;
            }
        }
    }
    fprintf(2,"%s: No such a file or directory",path);
    return NULL;
}

static int procfs_close(inode_t* inode){
    return 0;
}

static fsops_t procfs_ops={
    .init  =procfs_init,
    .lookup=procfs_lookup,
    .close =procfs_close,
};
/*
 * procfs_iopen
 * procfs_iclose
 * procfs_iread
 * procfs_iwrite
 * procfs_ilseek
 * procfs_imkdir
 * procfs_irmdir
 * procfs_ilink
 * procfs_unlink
 */
static device_t* get_dev(vfile_t* file){
    return (device_t*)file->inode->ptr;
}

static int procfs_iopen(vfile_t* file,int flags){
    file->offset=0;
    file->flags=flags;
    file->refcnt=1;
    file->lk=PTHREAD_MUTEX_INITIALIZER;
    return 0;
}

static int procfs_iclose(vfile_t* file){
    pthread_mutex_lock(&file->lk);
    --file->refcnt;
    if(file->refcnt==0){
        return file->inode->fs->ops->close(file->inode);
    }
    pthread_mutex_unlock(&file->lk);
    return 0;
}

static ssize_t procfs_iread(vfile_t* file,char* buf,size_t size){
    device_t* dev=get_dev(file);
    ssize_t ret  =dev->ops->read(dev,file->offset,buf,size);
    file->offset+=ret;
    return ret;
}

static ssize_t procfs_ireaddir(vfile_t* file,char* buf,size_t size){
    ssize_t nread;
    if(file->inode==&procfs_root){
        if(file->offset<0x40){
            if(tasks[file->offset]){
                nread=sprintf(buf,"%d",file->offset);
            }
            ++file->offset;
        }else{
            return 0;
        }
    }else{
        nread=0;
        TODO();
        return 0;
        uint8_t* p=file->inode->ptr;
        if(p[1]){
            warn("%s/%d/%s: Not a directory",procfs.mount,p[0],proc_info[p[1]]);
        }
    }
    return nread;
}

static ssize_t procfs_iwrite(vfile_t* file,const char* buf,size_t size){
    device_t* dev=get_dev(file);
    ssize_t ret  =dev->ops->write(dev,file->offset,buf,size);
    file->offset+=ret;
    return ret;
}

static ssize_t procfs_ilseek(vfile_t* file,off_t offset,int whence){
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

static ssize_t procfs_imkdir(const char* name){
    fprintf(STDERR,"cannot create directory ‘%s’: Read-only filesystem\n",name);
    return -1;
}

static ssize_t procfs_irmdir(const char* name){
    fprintf(STDERR,"cannot remove directory ‘%s’: Read-only filesystem\n",name);
    return -1;
}

static ssize_t procfs_ilink(const char* name, inode_t* inode){
    fprintf(STDERR,"cannot create file ‘%s’: Read-only filesystem\n",name);
    return -1;
}

static ssize_t procfs_iunlink(const char* name){
    fprintf(STDERR,"cannot remove file ‘%s’: Read-only filesystem\n",name);
    return -1;
}

//.func_name=dev_ifunc_name
//i for inode
static inodeops_t procfs_iops={
    .open   =procfs_iopen,
    .close  =procfs_iclose,
    .read   =procfs_iread,
    .readdir=procfs_ireaddir,
    .write  =procfs_iwrite,
    .lseek  =procfs_ilseek,
    .mkdir  =procfs_imkdir,
    .rmdir  =procfs_irmdir,
    .link   =procfs_ilink,
    .unlink =procfs_iunlink,
};

filesystem procfs={
    .ops     =&procfs_ops,
    .dev     =NULL,
    .inodeops=&procfs_iops,
};

static inode_t procfs_root={
    .ptr   =NULL,
    .fs    =&procfs,
    .ops   =&procfs_iops,
};


