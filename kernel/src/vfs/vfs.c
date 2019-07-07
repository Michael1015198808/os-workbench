#include <common.h>
#include <devices.h>
#include <shell.h>
#include <fs.h>

#define current currents[cpu_id]
static vfile_t** get_fd(void){
    intr_close();
    int cpu_id=_cpu();
    return current->fd;
    intr_open();
}
static int new_fd_num(int cpu_id){
    for(int i=0;i<FD_NUM;++i){
        if(!(current->fd[i])){
            return i;
        }
    }
    return -1;//No more file descripter (number)!
}
static inline int vfs_open_real(const char *path,int flags){
    int cpu_id=_cpu();
    int fd=new_fd_num(cpu_id);
    Assert(fd!=-1,"No more file descripter!");//Or return -1;
    current->fd[fd]=pmm->alloc(sizeof(vfile_t));
    if(path[0]=='/'){//Temporarily
        current->fd[fd]->type=VFILE_FILE;
        rd[0].ops->lookup(&rd[0],path,flags);
    }else{
        device_t *dev=dev_lookup(path);
        current->fd[fd]->type=VFILE_DEV;
        current->fd[fd]->ptr=dev;
    }
    return fd;
}
static int vfs_open(const char *path, int flags){
    _intr_close();
    int ret=vfs_open_real(path,flags);
    _intr_open();
    return ret;
}
static inline ssize_t vfs_read_real(int fd, void* buf,size_t nbyte){
    int cpu_id=_cpu();
    switch(current->fd[fd]->type){
        case VFILE_DEV:
            {
                device_t* dev=(device_t*)current->fd[fd]->ptr;
                ssize_t nread=dev->ops->read(dev,0,buf,nbyte);
                return nread;
            }
            break;
        case VFILE_FILE:
            TODO();
            break;
        case VFILE_PROC:
            TODO();
            break;
        case VFILE_MEM:
            {
                strncpy(buf,current->fd[fd]->ptr,nbyte);
                int ret=strlen(buf);
                current->fd[fd]->ptr+=ret;
                return ret;
            }
        case VFILE_NULL:
            return 0;
        default:
            Assert(0,"Unknown fd type:%d\n",current->fd[fd]->type);
            break;
    }
    Assert(0,"Should not reach here!\n");
}
static ssize_t vfs_read(int fd,void *buf,size_t nbyte){
    _intr_close();
    ssize_t ret=vfs_read_real(fd,buf,nbyte);
    _intr_open();
    return ret;
}
static inline ssize_t vfs_write_real(int fd,void *buf,size_t nbyte){
    int cpu_id=_cpu();
    switch(current->fd[fd]->type){
        case VFILE_DEV:
            {
                device_t *dev=current->fd[fd]->ptr;
                return dev->ops->write(dev,0,buf,nbyte);
            }
            break;
        case VFILE_FILE:
            TODO();
            break;
        case VFILE_PROC:
            TODO();
            break;
        case VFILE_MEM:
            memcpy(current->fd[fd]->ptr,buf,nbyte);
            current->fd[fd]->ptr+=nbyte;
            *(char*)current->fd[fd]->ptr='\0';
            return nbyte;
        case VFILE_NULL:
            return nbyte;
        default:
            Assert(0,"Unknown fd type:%d\n",current->fd[fd]->type);
            break;
    }
}
static ssize_t vfs_write(int fd,void* buf,size_t nbyte){
    intr_close();
    ssize_t ret=vfs_write_real( fd,buf,nbyte);
    intr_open();
    return ret;
}
static int vfs_exec(const char* file,void *args[]){
    int ret,is_buildin;
    ret=exec_buildin(file,args,&is_buildin);
    if(is_buildin){
        return ret;
    }else{
        std_write(args[0]);
        char warn[]=": command not found\n";
        std_write(warn);
        return -1;
    }
}
extern fsops_t yls_ops;
void vfs_init(void){
    rd[0].ops=&yls_ops;
    rd[0].dev=dev_lookup("ramdisk0");
    rd[0].ptr=NULL;
    rd[1].ops=&yls_ops;
    rd[1].dev=dev_lookup("ramdisk1");
    rd[1].ptr=NULL;
}
int vfs_access(const char *path, int mode){
    TODO();
}
int vfs_mount(const char *path, filesystem *fs){
    TODO();
}
int vfs_unmount(const char *path){
    TODO();
}
int vfs_mkdir(const char *path){
    TODO();
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
off_t vfs_lseek(int fd,off_t offset, int whence){
    TODO();
}
int vfs_close(int fd){
    TODO();
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
  .write    =vfs_write,
  .exec     =vfs_exec,
  .lseek    =vfs_lseek,
  .close    =vfs_close
};

ssize_t std_read(void *buf){
    return vfs->read(STDIN,buf,-1);
}
ssize_t std_write(void *buf){
    return vfs->write(STDOUT,buf,strlen(buf));
}
