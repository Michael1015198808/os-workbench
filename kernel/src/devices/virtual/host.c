#include <devices.h>

static int host_init(device_t *dev) {
    return 0;
}

static ssize_t host_read(device_t *dev, off_t offset, void *buf, size_t count) {
    extern uint8_t _getc(void);
    for(size_t i=0;i<count;++i){
        buf[i]=_getc();
    }
    return count;
}

static ssize_t host_write(device_t *dev, off_t offset, const void *buf, size_t count) {
    for(size_t i=0;i<count;++i){
        _putc(buf[i]);
    }
    return count;
}

devops_t host_ops = {
  .init = host_init,
  .read = host_read,
  .write = host_write,
};


