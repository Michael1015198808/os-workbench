#ifndef __VFS_H
#define __VFS_H

#include <amdev.h>
#include <klib.h>

#define STDIN  0
#define STDOUT 1
#define STDERR 2

#define O_RDONLY    0x01
#define O_WRONLY    0x02
#define O_CREATE    0x04
#define O_DIRECTORY 0x08
#define O_APPEND    0x10

#define O_RDWR (O_RDONLY | O_WRONLY)
#define O_CREAT    O_CREATE


#define SEEK_SET 0
#define SEEK_CUR 1
#define SEEK_END 2

#define EISDIR -1
#define ENOENT -1

struct filesystem;
typedef struct filesystem filesystem;
typedef struct vfile vfile_t;
typedef struct fsops fsops_t;
typedef struct inode inode_t;
typedef struct inodeops inodeops_t;
typedef struct path_pair path_pair;

typedef struct {
  void (*init)();
  int (*access)(const char *path, int mode);
  int (*mount)(const char *path, filesystem *fs);
  int (*unmount)(const char *path);
  int (*mkdir)(const char *path);
  int (*rmdir)(const char *path);
  int (*link)(const char *oldpath, const char *newpath);
  int (*unlink)(const char *path);
  int (*open)(const char *path, int flags);
  ssize_t (*read)(int fd, void *buf, size_t nbyte);
  ssize_t (*readdir)(int fd, void *buf, size_t nbyte);
  ssize_t (*write)(int fd, void *buf, size_t nbyte);
  int (*exec)(const char* file,void *args[]);
  off_t (*lseek)(int fd, off_t offset, int whence);
  int (*close)(int fd);
  int (*chdir)(const char*);
} MODULE(vfs);


ssize_t std_read(void *buf);
ssize_t std_write(void *buf);

struct vfile{
    uint32_t offset;
    inode_t* inode;
    int refcnt,flags;
    pthread_mutex_t lk;
};

struct inode {
  void *ptr;       // private data
  filesystem *fs;
  inodeops_t *ops; // 在inode被创建时，由文件系统的实现赋值
                   // inode ops也是文件系统的一部分
};

struct filesystem{
    const char *name;
    fsops_t *ops;
    //Call devops by fsops
    device_t *dev;
    inode_t* inodes,*root,*root_parent;
};

struct path_pair{
    const char* path;
    char* pwd;
    int idx;
};

struct fsops {
  void (*init)(struct filesystem *fs, const char *name, device_t *dev);
  inode_t *(*lookup)(struct filesystem *fs, const char *path, int flags);
  int (*close)(inode_t *inode);
} ;

struct inodeops {
  int (*open)(vfile_t *file, int flags);
  int (*close)(vfile_t *file);
  ssize_t (*read)(vfile_t *file, char *buf, size_t size);
  ssize_t (*readdir)(vfile_t *file, char *buf, size_t size);
  ssize_t (*write)(vfile_t *file, const char *buf, size_t size);
  off_t (*lseek)(vfile_t *file, off_t offset, int whence);
  int (*mkdir)( inode_t*,const char *name);
  int (*rmdir)( inode_t*,const char *name);
  int (*link)(  inode_t*, const char *name,inode_t*);
  int (*unlink)(inode_t*, const char *name);
  inode_t* (*find)(inode_t* cur,const char* name,int flags);
};

extern filesystem
    blkfs[2],//Ramdisk
    devfs,
    procfs;

static inline inode_t* vfs_find(inode_t* inode,const char* path,int flags){
    return inode->ops->find(inode,path,flags);
}

#endif//__VFS_H
