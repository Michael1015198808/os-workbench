#include <common.h>
#include <devices.h>
#include <shell.h>
#include <vfs.h>
#include <yls.h>
#include <dir.h>

#define this_fd current->fd[fd]

struct{
    const char* path;
    inode_t backup;
}mount_table[20];
#define mtt mount_table

int mount_table_cnt=0;
#define mtt_tab mtt[mount_table_cnt]
pthread_mutex_t mount_table_lk=PTHREAD_MUTEX_INITIALIZER;

int new_fd_num(task_t* current){
    for(int i=0;i<FD_NUM;++i){
        if(!(current->fd[i])){
            return i;
        }
    }
    return -1;//No more file descripter (number)!
}

static inode_t* vfs_root=NULL;

inode_t* vfs_lookup(const char* path,int flags);
void vfs_init(void){
    blkfs[0].ops->init  (blkfs+0    ,"ramdisk0" ,dev_lookup("ramdisk0") );
    blkfs[1].ops->init  (blkfs+1    ,"ramdisk1" ,dev_lookup("ramdisk1") );
    devfs.ops->init     (&devfs     ,"devfs"    ,NULL                   );
    procfs.ops->init    (&procfs    ,"procfs"   ,NULL                   );

    vfs->mount("/"      ,&blkfs[0]);
    vfs->mount("/mnt/"   ,&blkfs[1]);
    vfs->mount("/dev/"   ,&devfs);
    vfs->mount("/proc/"  ,&procfs);
}

int vfs_access(const char *path, int mode){
    return vfs_lookup(path,mode)==NULL;
}

//Paths end up without / is also support
int vfs_mount(const char *path, filesystem *fs){
    pthread_mutex_lock(&mount_table_lk);

    if(strcmp(path,"/")){
        inode_t* origin=vfs_lookup(path,O_RDONLY|O_DIRECTORY);
        //Replace origin inode at path
        if(origin==NULL){
            TODO();
        }
        mtt_tab=( typeof(mtt_tab) ){
            .path=path,
            .backup=*origin,
        };
        *origin =*fs->root;
        ++mount_table_cnt;

        char root_parent[0x100];
        strcpy(root_parent,path);
        dir_cat(root_parent,"..");
        fs->root_parent=vfs_lookup(root_parent,O_RDONLY|O_DIRECTORY);
    }else{
        vfs_root=fs->root_parent=fs->root;
    }

    pthread_mutex_unlock(&mount_table_lk);
    return 0;
}
int vfs_unmount_real(const char *path){
    for(int i=0;i<mount_table_cnt;++i){
        if(!strcmp(path,mount_table[i].path)){
            --mount_table_cnt;
            *vfs_lookup(path,O_RDONLY|O_DIRECTORY)=mount_table[i].backup;
            mount_table[i]=mount_table[mount_table_cnt];
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

inode_t* vfs_lookup(const char* path,int flags){
    inode_t* start=NULL;
    if(*path=='/'){
        start=vfs_root;
    }else{
        start=get_cur()->cur_dir;
    }
    return start->ops->find(start,path,flags);
}

int vfs_mkdir(const char* path){
    if(vfs_lookup(path,O_RDONLY)){
        warn("cannot create directory '%s': File exists",path);
        return -1;
    }else{
        clear_warn();
        return vfs_lookup(path,O_RDONLY|O_CREAT|O_DIRECTORY)==NULL;
    }
}

static inline inode_t* get_parent(const char**s,char* new_parent){
    const char* path=*s;
    int len=get_last_slash(path)+1;
    *s=path+len;
    if(len==0){
        return get_cur()->cur_dir;
    }else{
        strncpy(new_parent,path,len);
        return vfs_lookup(new_parent,O_RDONLY);
    }
}

int vfs_rmdir(const char *path){
    char new_parent[0x100];

    inode_t* parent=get_parent(&path,new_parent);

    if(!parent){
        warn("No such file or directory",new_parent);
    }else if(parent->ops->find(parent,path,O_RDONLY)){
        return parent->ops->rmdir(parent,path);
    }else{
        warn("No such file or directory",new_parent);
    }
    return 0;
}

int vfs_link(const char *oldpath, const char *newpath){
    const char* const ori_newpath=newpath;
    char new_parent[0x100];

    inode_t* parent=get_parent(&newpath,new_parent);

    if(!parent){
        warn("No such file or directory %s",new_parent);
    }else if(parent->ops->find(parent,newpath,O_RDONLY)){
        warn("File exist",ori_newpath,oldpath);
    }else{
        clear_warn();//Clear warn for finding 
        inode_t* old=vfs_lookup(oldpath,O_RDONLY);
        if(!old){
            warn("%s does not exists",oldpath);
        }else if(parent->fs->dev!=old->fs->dev){
            warn("Invalid cross-device link");
        }else{
            return parent->ops->link(parent,newpath,old);
        }
    }
    return -1;
}

int vfs_unlink(const char *path){
    char new_parent[0x100];

    inode_t* parent=get_parent(&path,new_parent);

    if(!parent){
        warn("No such file or directory",new_parent);
    }else if(parent->ops->find(parent,path,O_RDONLY)){
        return parent->ops->unlink(parent,path);
    }else{
        warn("No such file or directory",new_parent);
    }
    return -1;
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

int vfs_chdir(const char* path){
    inode_t* next=NULL;
    next=vfs_lookup(path,O_RDONLY|O_DIRECTORY);
    if(next){
        task_t* cur=get_cur();
        cur->cur_dir=next;
        dir_cat(cur->pwd,path);
        return 0;
    }
    return -1;
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
  .chdir    =vfs_chdir,
};

ssize_t std_read(void *buf){
    return vfs->read(STDIN,buf,-1);
}
ssize_t std_write(void *buf){
    return vfs->write(STDOUT,buf,strlen(buf));
}
