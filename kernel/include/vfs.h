#ifndef __VFS_H
#define __VFS_H

#include <amdev.h>
#include <klib.h>

#define STDIN  0
#define STDOUT 1
#define STDERR 2

#define O_RDONLY    1
#define O_WRONLY    2
#define O_CREATE    4
#define O_DIRECTORY 8

#define O_RDWR (O_RDONLY | O_WRONLY)

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
} MODULE(vfs);

ssize_t std_read(void *buf);
ssize_t std_write(void *buf);

#define VFILE_FILE  1
#define VFILE_DEV   2
#define VFILE_PROC  3
#define VFILE_MEM   4
//Read/Write to memory directly
//can be used for pipe
#define VFILE_NULL  5
//Read from it always returns 0(Work as EOF)
//Write to it always returns nbyte
struct vfile{
    uint32_t offset;
    inode_t* inode;
    int refcnt,flags;
    pthread_mutex_t lk;
};

struct filesystem{
    const char* name;
    fsops_t *ops;
    //Call devops by fsops
    device_t *dev;
    inode_t* inodes;
    inodeops_t* inodeops;
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
  int (*mkdir)(const char *name);
  int (*rmdir)(const char *name);
  int (*link)(const char *name, inode_t *inode);
  int (*unlink)(const char *name);
};

struct inode {
  void *ptr;       // private data
  filesystem *fs;
  inodeops_t *ops; // 在inode被创建时，由文件系统的实现赋值
                   // inode ops也是文件系统的一部分
};

extern filesystem
    blkfs[2],//Ramdisk
    devfs;

#endif//__VFS_H
