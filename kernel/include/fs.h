#ifndef __FS_H
#define __FS_H
typedef struct fsops fsops_t;

typedef struct filesystem{
  fsops_t *ops;
  dev_t *dev;
}filesystem;

struct fsops {
  void (*init)(struct filesystem *fs, const char *name, dev_t *dev);
  inode_t *(*lookup)(struct filesystem *fs, const char *path, int flags);
  int (*close)(inode_t *inode);
} ;

typedef struct inodeops {
  int (*open)(file_t *file, int flags);
  int (*close)(file_t *file);
  ssize_t (*read)(file_t *file, char *buf, size_t size);
  ssize_t (*write)(file_t *file, const char *buf, size_t size);
  off_t (*lseek)(file_t *file, off_t offset, int whence);
  int (*mkdir)(const char *name);
  int (*rmdir)(const char *name);
  int (*link)(const char *name, inode_t *inode);
  int (*unlink)(const char *name);
  // 你可以自己设计readdir的功能
} inodeops_t;

struct inode {
  int refcnt;
  void *ptr;       // private data
  filesystem *fs;
  inodeops_t *ops; // 在inode被创建时，由文件系统的实现赋值
                   // inode ops也是文件系统的一部分
};

typedef struct file {
  int refcnt; // 引用计数
  inode_t *inode;
  uint64_t offset;
} file_t;

#endif//__FS_H
