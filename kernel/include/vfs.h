#ifndef __VFS_H
#define __VFS_H

#include <amdev.h>
#include <klib.h>

#define STDIN  0
#define STDOUT 1
#define STDERR 2

#define O_RDONLY 1
#define O_WRONLY 2
#define O_CREATE 4

#define O_RDWR (O_RDONLY | O_WRONLY)

struct filesystem;
typedef struct filesystem filesystem;

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
    void *ptr;
    int type;
};
typedef struct vfile vfile_t;
#endif
