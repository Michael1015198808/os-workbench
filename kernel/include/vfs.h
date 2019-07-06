#ifndef __VFS_H
#define __VFS_H

#include <common.h>
#include <amdev.h>
#include <klib.h>

typedef struct {
  void (*init)();
  int (*access)(const char *path, int mode);
  int (*mount)(const char *path, filesystem_t *fs);
  int (*unmount)(const char *path);
  int (*mkdir)(const char *path);
  int (*rmdir)(const char *path);
  int (*link)(const char *oldpath, const char *newpath);
  int (*unlink)(const char *path);
  int (*open)(const char *path, int flags);
  ssize_t (*read)(int fd, void *buf, size_t nbyte);
  ssize_t (*write)(int fd, void *buf, size_t nbyte);
  off_t (*lseek)(int fd, off_t offset, int whence);
  int (*close)(int fd);
} MODULE(vfs);

#define VFILE_FILE 1
#define VFILE_DEV 2
#define VFILE_PROC 3
typedef struct {
    void *actual;
    int type;
}vfile_t;
#endif
