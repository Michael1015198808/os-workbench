#include <devices.h>

extern char initrd_start, initrd_end;

int rd_init(device_t *dev) {
  rd_t *rd = dev->ptr;
  if (dev->id == 1) {
    rd->start = &initrd_start;
    rd->end   = &initrd_end;
  } else {
    char *space = pmm->alloc(RD_SIZE);
    rd->start = space;
    rd->end   = space + RD_SIZE;
  }
  return 0;
}

ssize_t rd_read(device_t *dev, off_t offset, void *buf, size_t count) {
  rd_t *rd = dev->ptr;
  memcpy(buf, ((char *)rd->start) + offset, count);
  return count;
}

ssize_t rd_write(device_t *dev, off_t offset, const void *buf, size_t count) {
  rd_t *rd = dev->ptr;
  memcpy(((char *)rd->start) + offset, buf, count);
  return count;
}

devops_t rd_ops = {
  .init = rd_init,
  .read = rd_read,
  .write = rd_write,
};
