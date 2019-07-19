#include <common.h>
#include <devices.h>
#include <shell.h>
#include <vfs.h>
#include <yls.h>

#define this_fd current->fd[fd]

struct{
    const char* path;
    filesystem* fs;
}mount_table[20];
int mount_table_cnt=0;
pthread_mutex_t mount_table_lk=PTHREAD_MUTEX_INITIALIZER;

int new_fd_num(task_t* current){
    for(int i=0;i<FD_NUM;++i){
        if(!(current->fd[i])){
            return i;
        }
    }
    return -1;//No more file descripter (number)!
}

static inline inode_t* vfs_lookup(const char* path,int flags);
void vfs_init(void){
    blkfs[0].ops->init  (blkfs+0    ,"ramdisk0" ,dev_lookup("ramdisk0") );
  //blkfs[1].ops->init  (blkfs+1    ,"ramdisk1" ,dev_lookup("ramdisk1") );
    devfs.ops->init     (&devfs     ,"devfs"    ,NULL                   );
    procfs.ops->init    (&procfs    ,"procfs"   ,NULL                   );

    vfs->mount("/"      ,&blkfs[0]);
    vfs->mount("/mnt/"   ,&blkfs[1]);
    vfs->mount("/dev/"   ,&devfs);
    devfs.root_parent=vfs_lookup("/",O_RDONLY|O_DIRECTORY);
    vfs->mount("/proc/"  ,&procfs);
}

int vfs_access(const char *path, int mode){
    return 0;
}
//Path SHOULD all end with /
//"/dev/ramdisk0" will be transformed into "/ramdisk0"
int vfs_mount(const char *path, filesystem *fs){
    pthread_mutex_lock(&mount_table_lk);
    mount_table[mount_table_cnt].path=path;
    mount_table[mount_table_cnt].fs=fs;
    ++mount_table_cnt;
    pthread_mutex_unlock(&mount_table_lk);
    return 0;
}
int vfs_unmount_real(const char *path){
    for(int i=0;i<mount_table_cnt;++i){
        if(!strcmp(path,mount_table[i].path)){
            --mount_table_cnt;
            mount_table[i].path=mount_table[mount_table_cnt].path;
            mount_table[i].fs  =mount_table[mount_table_cnt].fs  ;
            return 0;
        }
    }
    return -1;
}
int vfs_unmount(const char* path){
    pthread_mutex_lock(&mount_table_lk);
    int ret=vfs_unmount_real(path);
    pthread_mutex_unlock(&mount_table_lk);
    return ret;
}
int vfs_mkdir(const char* path){
    TODO();
    /*
    extern inodeops_t yls_iops;
    yls_node cur;

    inode_t inode;
    inode.ptr=&cur;
    inode.fs=&rd[0];
    inode.ops=&yls_iops;
    inode.offset=0;

    vfile_t vfile;
    vfile.ptr   =&inode;
    vfile.type  =VFILE_FILE;
    //
    extern inodeops_t yls_iops;
    Assert(inode->ops==&yls_iops,
            "Something wrong happens when try to open /");
            //

    struct{
        uint32_t type;
        const char* path;
    }info;

    extern ssize_t rd_read(device_t *dev, off_t offset, void *buf, size_t count);
    extern device_t *devices[];
    rd_read(devices[0],HEADER_LEN,&cur,12);
    int pos=find_path(devices[0],&inode,path);

    info.type=YLS_DIR;
    info.path=path+pos;
    printf("%s\n%s\n",path,info.path);

    return inode.ops->write(&vfile,(void*)&info,strlen(path));
    */
}
int vfs_rmdir(const char *path){
    TODO();
}
int vfs_link(const char *oldpath, const char *newpath){
    TODO();
}
int vfs_unlink(const char *path){
    TODO();
}
static inline inode_t* vfs_lookup(const char* path,int flags){
    pthread_mutex_lock(&mount_table_lk);
    filesystem* target=NULL;
    int max_len=-1;
    for(int i=0;i<mount_table_cnt;++i){
        int len=strlen(mount_table[i].path)-1;//Avoid multi-calls
        if( !strncmp(mount_table[i].path,path,len) &&
             (path[len]=='/'||path[len]=='\0')   ){
            if(len>max_len){
                max_len=len;
                target=mount_table[i].fs;
            }
        }
    }
    pthread_mutex_unlock(&mount_table_lk);
    return target->ops->lookup(target,path+max_len,flags);
}
static inline int vfs_open_real(const char *path,int flags){
    task_t* current=get_cur();

    int fd=new_fd_num(current);
    Assert(fd!=-1,"No more file descripter!");//Or return -1;

    inode_t* inode=vfs_lookup(path,flags);
    if(!inode)return -1;
    this_fd=pmm->alloc(sizeof(vfile_t));

    this_fd->inode=inode;
    this_fd->inode->ops->open(this_fd,flags);
    return fd;
}

static int vfs_open(const char *path, int flags){
    int ret=vfs_open_real(path,flags);
    return ret;
}

static inline ssize_t vfs_read_real(int fd, void* buf,size_t nbyte){
    task_t* current=get_cur();
    if(this_fd){
        return this_fd->inode->ops->read(this_fd,buf,nbyte);;
    }else{
        return 0;
    }
}

static ssize_t vfs_read(int fd,void *buf,size_t nbyte){
    ssize_t ret=vfs_read_real(fd,buf,nbyte);
    return ret;
}

static inline ssize_t vfs_readdir_real(int fd, void* buf,size_t nbyte){
    task_t* current=get_cur();
    if(this_fd){
        return this_fd->inode->ops->readdir(this_fd,buf,nbyte);;
    }else{
        return 0;
    }
}

static ssize_t vfs_readdir(int fd,void *buf,size_t nbyte){
    ssize_t ret=vfs_readdir_real(fd,buf,nbyte);
    return ret;
}

static inline ssize_t vfs_write_real(int fd,void *buf,size_t nbyte){
    task_t* current=get_cur();
    if(this_fd){
        return this_fd->inode->ops->write(this_fd,buf,nbyte);
    }else{
        return 0;
    }
}

static ssize_t vfs_write(int fd,void* buf,size_t nbyte){
    ssize_t ret=vfs_write_real(fd,buf,nbyte);
    return ret;
}

static int vfs_exec(const char* file,void *args[]){
    int ret,is_buildin;
    ret=exec_buildin(file,args,&is_buildin);
    if(is_buildin){
        return ret;
    }else{
        fprintf(STDERR,"%s: command not found\n",args[0]);
        return -1;
    }
}

off_t vfs_lseek(int fd,off_t offset, int whence){
    TODO();
}

int vfs_close(int fd){
    task_t* current=get_cur();
    this_fd->inode->ops->close(this_fd);
    Assert(this_fd->refcnt>=0,"fd with refcnt <0!\n");
    if(this_fd->refcnt==0){
        pmm->free(this_fd);
    }
    this_fd=NULL;
    return 0;
}

static inode_t* vfs_find(inode_t* inode,const char* path){
    if(!inode)
        warn("No such a file or directory");
    while(*path=='/')++path;
    if(!*path)return (inode_t*)inode;
    inode->ops->find(inode,path);
}
MODULE_DEF(vfs){
  .init     =vfs_init,
  .access   =vfs_access,
  .mount    =vfs_mount,
  .unmount  =vfs_unmount,
  .mkdir    =vfs_mkdir,
  .rmdir    =vfs_rmdir,
  .link     =vfs_link,
  .unlink   =vfs_unlink,
  .open     =vfs_open,
  .read     =vfs_read,
  .readdir  =vfs_readdir,
  .write    =vfs_write,
  .exec     =vfs_exec,
  .lseek    =vfs_lseek,
  .close    =vfs_close,
  .find     =vfs_find,
};

ssize_t std_read(void *buf){
    return vfs->read(STDIN,buf,-1);
}
ssize_t std_write(void *buf){
    return vfs->write(STDOUT,buf,strlen(buf));
}
