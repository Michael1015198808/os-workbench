#include <devices.h>
#include <common.h>
#include <vfs.h>
/*
 * procfs_init
 * procfs_lookup
 * dprocs_close
 */

extern task_t *tasks[0x40];

#define PROC_DIR  0
#define PROC_PWD  1
#define PROC_NAME 2

static const char* other_info[]={
    "devices",
    "meminfo",
    "uptime",
};
const char* per_task_info[]={
    "",
    "pwd",
    "name",
};

void other_info_init(filesystem* fs);

static inodeops_t procfs_rootiops;
static inodeops_t procfs_taskiops;
static inodeops_t procfs_iops;
static uint8_t num[2]={0,0};

static void procfs_init(filesystem* fs,const char* name,device_t *dev){
    fs->name=name;
    fs->dev=dev;
    fs->inodes=pmm->alloc(sizeof(inode_t)*
            ((0x40*3)+
            //3 inodes per process
            (LEN(other_info))) );

    fs->root  =pmm->alloc(sizeof(inode_t));
    *fs->root =(inode_t){
            .ptr   =num,
            .fs    =fs,
            .ops   =&procfs_rootiops,
        };
    for(int i=0;i<0x40;++i){
        for(int j=0;j<3;++j){
            int idx=i*3+j;
            fs->inodes[idx].ptr=pmm->alloc(sizeof(uint8_t)*2);
            uint8_t *p=fs->inodes[idx].ptr;
            p[0]=i;p[1]=j;
            fs->inodes[idx].fs=fs;
            if(j==0){
                fs->inodes[idx].ops=&procfs_taskiops;
            }else{
                fs->inodes[idx].ops=&procfs_iops;
            }
        }
    }
    other_info_init(fs);
}

static inode_t* procfs_lookup(filesystem* fs,const char* path,int flags){
    if((!path[0]))return procfs.root;
    return vfs_find(procfs.root,path,flags);
}

static int procfs_close(inode_t* inode){
    return 0;
}

static fsops_t procfs_ops={
    .init  =procfs_init,
    .lookup=procfs_lookup,
    .close =procfs_close,
};

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

static ssize_t procfs_rootiread(vfile_t* file,char* buf,size_t size){
    return EISDIR;
}

static ssize_t procfs_iread(vfile_t* file,char* buf,size_t size){
    ssize_t nread=0;
    uint8_t* p=file->inode->ptr;
    task_t* task=tasks[p[0]];
    if(task){
        switch(p[1]){
            case PROC_DIR:
                BARRIER();
                break;
            case PROC_PWD:
                file->offset+=
                    (nread+=snprintf(buf,size,task->pwd+file->offset));
                break;
            case PROC_NAME:
                file->offset+=
                    (nread+=snprintf(buf,size,task->name+file->offset));
                break;
            default:
                warn(":Unknown file type");
                break;
        }
    }
    return nread;
}

static ssize_t procfs_rootireaddir(vfile_t* file,char* buf,size_t size){
    ssize_t nread=0;
    while(file->offset<0x40&&!tasks[file->offset]){
        ++file->offset;
    }
    if(file->offset<0x40){
        nread=sprintf(buf,"%d",file->offset);
        ++file->offset;
    }else if(file->offset-0x40<LEN(other_info)){
        nread=sprintf(buf,"%s",other_info[file->offset-0x40]);
        ++file->offset;
    }
    return nread;
}

static ssize_t procfs_taskireaddir(vfile_t* file,char* buf,size_t size){
    ssize_t nread=0;
    if(file->offset<3){
        file->offset+=
            (nread=snprintf(buf,size,per_task_info[1]+file->offset));
    }else if(file->offset<7){
        file->offset+=
            (nread=snprintf(buf,size,per_task_info[2]+(file->offset-3)));
    }
    return nread;
}
static ssize_t procfs_ireaddir(vfile_t* file,char* buf,size_t size){
    warn("Not a directory");
    return 0;
}

static ssize_t procfs_iwrite(vfile_t* file,const char* buf,size_t size){
    error("Read only filesystem");
    return 0;
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

static ssize_t procfs_imkdir(inode_t* inode,const char* name){
    fprintf(STDERR,"cannot create directory '%s': Read-only filesystem\n",name);
    return -1;
}

static ssize_t procfs_irmdir(inode_t* inode,const char* name){
    fprintf(STDERR,"cannot remove directory '%s': Read-only filesystem\n",name);
    return -1;
}

static ssize_t procfs_ilink(inode_t* parent,const char* name, inode_t* inode){
    fprintf(STDERR,"cannot create file '%s': Read-only filesystem\n",name);
    return -1;
}

static ssize_t procfs_iunlink(inode_t* parent,const char* name){
    fprintf(STDERR,"cannot remove file '%s': Read-only filesystem\n",name);
    return -1;
}

static inode_t* procfs_rootifind(inode_t* cur,const char* path,int flags){
    inode_t* next=NULL;

    do{
        inode_t* inode=cur;
        while(*path=='/')++path;
        if(!*path){
            return (inode_t*)inode;
        }
    }while(0);

    const filesystem* fs=cur->fs;

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
    if(i==0){
        //A word
        for(int i=0;i<LEN(other_info);++i){
            int len=strlen(other_info[i]);
            if(!strncmp(path,other_info[i],len)){
                next=fs->inodes+(0x40*3+i);
                path+=len;
                break;
            }
        }
        if(path[0]=='.'){
            if(path[1]=='.'){
                //.. for parent
                next=procfs.root_parent;
                path+=2;
            }else{
                //. for current
                next=cur;
                ++path;
            }
        }
    }else if(i<3){
        //A proper pid
        int id=atoi(num);
        if(tasks[id]){
            id*=3;
            next=procfs.inodes+id;
        }
    }

    if(!next){
        warn("No such a file or directory");
        return NULL;
    }

    return next->ops->find(next,path,flags);
}

static inode_t* procfs_taskifind(inode_t* cur,const char* path,int flags){
    inode_t* next=NULL;

    do{
        inode_t* inode=cur;
        while(*path=='/')++path;
        if(!*path){
            return (inode_t*)inode;
        }
    }while(0);

    uint8_t *p=cur->ptr;
    Assert(p[1]==PROC_DIR);
    if(path[0]=='.'){
        if(path[1]=='.'){
            //.. for parent
            next=procfs.root;
            path+=2;
        }else{
            //. for current
            next=cur;
            ++path;
        }
    }else{
        for(int i=2;i>0;--i){
            int len=strlen(per_task_info[i]);
            if(!strncmp(path,per_task_info[i],len)){
                next=cur+i;
                path+=len;
                break;
            }
        }
    }

    if(!next){
        warn("No such a file or directory");
        return NULL;
    }
    return next->ops->find(next,path,flags);
}

static inode_t* procfs_ifind(inode_t* cur,const char* path,int flags){
    do{
        inode_t* inode=cur;
        if(*path=='/'){
            warn("Not a directory");
            return NULL;
        }
        if(!*path){
            if(flags & O_DIRECTORY){
                warn("Not a directory");
                return NULL;
            }
            else return (inode_t*)inode;
        }
    }while(0);

    warn("No such a file or directory");
    return NULL;
}

static inodeops_t procfs_rootiops={
    .open   =procfs_iopen,
    .close  =procfs_iclose,
    .read   =procfs_rootiread,
    .readdir=procfs_rootireaddir,
    .write  =procfs_iwrite,
    .lseek  =procfs_ilseek,
    .mkdir  =procfs_imkdir,
    .rmdir  =procfs_irmdir,
    .link   =procfs_ilink,
    .unlink =procfs_iunlink,
    .find   =procfs_rootifind,
};
static inodeops_t procfs_taskiops={
    .open   =procfs_iopen,
    .close  =procfs_iclose,
    .read   =procfs_rootiread,
    .readdir=procfs_taskireaddir,
    .write  =procfs_iwrite,
    .lseek  =procfs_ilseek,
    .mkdir  =procfs_imkdir,
    .rmdir  =procfs_irmdir,
    .link   =procfs_ilink,
    .unlink =procfs_iunlink,
    .find   =procfs_taskifind,
};
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
    .find   =procfs_ifind,
};

filesystem procfs={
    .ops     =&procfs_ops,
    .dev     =NULL,
};


static ssize_t devices_read(vfile_t* file,char* buf,size_t size);
static ssize_t meminfo_read(vfile_t* file,char* buf,size_t size);
static ssize_t uptime_read(vfile_t* file,char* buf,size_t size);

void other_info_init(filesystem* fs){
    for(int i=0;i<LEN(other_info);++i){
        fs->inodes[0x40*3+i].ptr=NULL;
        fs->inodes[0x40*3+i].fs=fs;
        fs->inodes[0x40*3+i].ops=pmm->alloc(sizeof(inodeops_t));
        *fs->inodes[0x40*3+i].ops=procfs_iops;
    }
    fs->inodes[0x40*3].ops->read=devices_read;
    fs->inodes[0x40*3+1].ops->read=meminfo_read;
    fs->inodes[0x40*3+2].ops->read=uptime_read;
}
static ssize_t devices_read(vfile_t* file,char* buf,size_t size){
    extern const device_t *devices[];
    extern const size_t devices_cnt;

    ssize_t nread=0;

    if(file->offset<devices_cnt){
        //Assume that buf can contains one whole line
        nread=snprintf(buf,size,"%2d %s\n",file->offset,devices[file->offset]->name);
        ++file->offset;
    }
    return nread;
}
static ssize_t meminfo_read(vfile_t* file,char* buf,size_t size){
    char info[0x40];
    uint32_t num[3];
    ssize_t nread=0;

    void mem_query(uint32_t num[3]);
    mem_query(num);
    double rate=num[1];
    rate/=num[0];
    rate*=100;
    sprintf(info,"MemTotal: %10d kB\nMemUsed:  %10d kB(%f%%)\n",num[0],num[1],rate);
    file->offset+=
        (nread=snprintf(buf,size,"%s",info+file->offset));
    return nread;
}
static ssize_t uptime_read(vfile_t* file,char* buf,size_t size){
    if(file->offset)return 0;
    else ++file->offset;
    return snprintf(buf,size,"%d\n",uptime());
}
