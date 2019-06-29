#include <common.h>
#include <amtrace.h>
#include <klib.h>
#include <devices.h>

static struct Command{
    const char *name,
    (int)(*const binary(void*));
}commands[]={
};
void mysh(void *name) {
  device_t *tty = dev_lookup(name);
  while (1) {
    char line[128], text[128];
    sprintf(text, "(%s) $ ", name); tty->ops->write(tty, 0, text, strlen(text)+1);
    int nread = tty->ops->read(tty, 0, line, sizeof(line));
    line[nread - 1] = '\0';
    sprintf(text, "Echo: %s.\n", line); tty->ops->write(tty, 0, text, strlen(text)+1);
  }
}
