#ifndef __FS_H
#define __FS_H
typedef struct fsops fsops_t;
typedef struct vfile vfile_t;
typedef struct inode inode_t;
typedef struct inodeops inodeops_t;
typedef struct filesystem filesystem;

struct filesystem{
    fsops_t *ops;
    //Call devops by fsops
    device_t *dev;
    void* ptr;
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
  ssize_t (*write)(vfile_t *file, const char *buf, size_t size);
  off_t (*lseek)(vfile_t *file, off_t offset, int whence);
  int (*mkdir)(const char *name);
  int (*rmdir)(const char *name);
  int (*link)(const char *name, inode_t *inode);
  int (*unlink)(const char *name);
  // 你可以自己设计readdir的功能
};

struct inode {
  void *ptr;       // private data
  filesystem *fs;
  inodeops_t *ops; // 在inode被创建时，由文件系统的实现赋值
                   // inode ops也是文件系统的一部分
  int offset;
};

filesystem rd[2];//Ramdisk
#endif//__FS_H
