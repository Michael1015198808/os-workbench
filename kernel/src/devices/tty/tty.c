#include <devices.h>

#define TTY_COOK_BUF_SZ 1024

struct character tty_defaultch() {
  return (struct character) { .metadata = 0, .ch = '\0' };
}

// tty state changes
// ------------------------------------------------------------------

static void tty_enqueue(struct tty_queue *q, char ch) {
  *(q->rear++) = ch;
  if (q->rear == q->end) {
    q->rear = q->buf;
  }
  if (q->rear == q->front) panic("tty queue full");
  *q->rear = '\0';
}

static int tty_pop_back(struct tty_queue *q) {
  if (q->front != q->end) {
    char *pos = q->rear;
    if (pos == q->buf) pos = q->end;
    if (*(pos - 1) != '\0') {
      q->rear = pos - 1;
      *q->rear = '\0';
      return 0;
    }
  }
  return 1;
}

static void tty_upd_scrollup(tty_t *tty) {
   int move_sz = tty->columns * (tty->lines - 1);
   memmove(tty->buf, tty->buf + tty->columns, move_sz * sizeof(tty->buf[0]));
   memmove(tty->dirty, tty->dirty + tty->columns, move_sz * sizeof(tty->dirty[0]));
   tty->cursor -= tty->columns;
   for (int i = 0; i < tty->columns; i++) {
     tty->cursor[i] = tty_defaultch();
   }
}

static inline void tty_upd_cr(tty_t *tty) {
  int x = (tty->cursor - tty->buf) % tty->columns;
  tty->cursor -= x;
}

static inline void tty_upd_lf(tty_t *tty) {
  tty->cursor += tty->columns;
}

static inline void tty_upd_backsp(tty_t *tty) {
  if (tty->cursor > tty->buf) {
    tty->cursor--;
    tty->cursor->ch = '\0';
  }
}

static inline void tty_upd_putc(tty_t *tty, char ch) {
  tty->cursor->ch = ch;
  tty->cursor++;
}

static int tty_cook(tty_t *tty, char ch) {
  int ret = 0;
  kmt->sem_wait(&tty->lock);
  struct tty_queue *q = &tty->queue;
  switch (ch) {
    case '\n':
      tty_enqueue(q, ch);
      tty_enqueue(q, '\0');
      kmt->sem_signal(&tty->cooked);
      break;
    case '\b':
      ret = tty_pop_back(q);
      break;
    default:
      tty_enqueue(q, ch);
  }
  kmt->sem_signal(&tty->lock);
  return ret;
}

// tty marking
// ------------------------------------------------------------------

static void tty_render(tty_t *tty) {
  struct character *ch = tty->buf;
  uint8_t *d = tty->dirty;
  kmt->sem_wait(&tty->lock);
  for (int y = 0; y < tty->lines; y++) {
    for (int x = 0; x < tty->columns; x++) {
      if (*d) {
        int draw = (ch == tty->cursor) ? 0xdb : ch->ch;
        struct sprite sp[2] = {
          { .x = x * 8, .y = y * 16, .z = 0,
            .display = tty->display, .texture = draw * 2 + 1, },
          { .x = x * 8, .y = y * 16 + 8, .z = 0,
            .display = tty->display, .texture = draw * 2 + 2, },
        };
        tty->fbdev->ops->write(tty->fbdev, SPRITE_BRK, &sp, sizeof(sp));
        }
      ch++; d++;
    }
  }
  kmt->sem_signal(&tty->lock);
}

static void tty_mark(tty_t *tty, struct character *ch) {
  tty->dirty[ch - tty->buf] = 1;
}

static void tty_mark_line(tty_t *tty, struct character *ch) {
  int x = (ch - tty->buf) % tty->columns;
  for (int i = 0; i < tty->columns; i++)
    tty_mark(tty, ch - x + i);
}

static void tty_mark_all(tty_t *tty) {
   for (int i = 0; i < tty->size; i++) {
     tty_mark(tty, &tty->buf[i]);
   }
}

// tty implementation
// ------------------------------------------------------------------

static void tty_putc(tty_t *tty, char ch) {
  switch (ch) {
    case '\r':
      tty_upd_cr(tty);
      tty_mark_line(tty, tty->cursor);
      break;
    case '\b':
      tty_upd_backsp(tty);
      tty_mark(tty, tty->cursor);
      tty_mark(tty, tty->cursor + 1);
      break;
    case '\n':
      tty_upd_cr(tty);
      tty_upd_lf(tty);
      if (tty->cursor == tty->end) {
        tty_upd_scrollup(tty);
        tty_mark_all(tty);
      } else {
        tty_mark_line(tty, tty->cursor - tty->columns);
        tty_mark_line(tty, tty->cursor);
      }
      break;
    default:
      tty_upd_putc(tty, ch);
      if (tty->cursor == tty->end) {
        tty_upd_scrollup(tty);
        tty_mark_all(tty);
      } else {
        tty_mark(tty, tty->cursor - 1);
        tty_mark(tty, tty->cursor);
      }
  }
}

int tty_init(device_t *dev) {
  tty_t *tty = dev->ptr;
  tty->fbdev = dev_lookup("fb");
  fb_t *fb = tty->fbdev->ptr;
  tty->display = dev->id - 1; // tty1 is on display #0
  tty->lines = fb->info->height / 16;
  tty->columns = fb->info->width / 8;
  tty->size = tty->columns * tty->lines;
  tty->buf = pmm->alloc(tty->size * sizeof(tty->buf[0]));
  tty->dirty = pmm->alloc(tty->size * sizeof(tty->dirty[0]));
  tty->end = tty->buf + tty->size;
  for (int i = 0; i < tty->size; i++) {
    tty->buf[i] = tty_defaultch();
    tty->dirty[i] = 1;
  }
  tty->cursor = tty->buf;
  struct tty_queue *q = &tty->queue;
  q->front = q->rear = q->buf = pmm->alloc(TTY_COOK_BUF_SZ);
  q->end = q->buf + TTY_COOK_BUF_SZ;
  kmt->sem_init(&tty->lock, "tty lock", 1);
  kmt->sem_init(&tty->cooked, "tty cooked lines", 0);
  return 0;
}

ssize_t tty_read(device_t *dev, off_t offset, void *buf, size_t count) {
  tty_t *tty = dev->ptr;
  kmt->sem_wait(&tty->cooked);
  kmt->sem_wait(&tty->lock);
  size_t nread = 0;

  struct tty_queue *q = &tty->queue;
  while (1) {
    char ch = *q->front;
    if (nread < count && ch != '\0') {
      ((char *)buf)[nread] = *q->front;
    }
    q->front++;
    if (q->front == q->end) q->front = q->buf;
    if (ch == '\0') break;
    nread++;
  }
  kmt->sem_signal(&tty->lock);
  return nread;
}

ssize_t tty_write(device_t *dev, off_t offset, const void *buf, size_t count) {
  tty_t *tty = dev->ptr;
  kmt->sem_wait(&tty->lock);
  for (size_t i = 0; i < count; i++) {
    tty_putc(tty, ((const char *)buf)[i]);
  }
  kmt->sem_signal(&tty->lock);
  tty_render(tty);
  return count;
}

devops_t tty_ops = {
  .init = tty_init,
  .read = tty_read,
  .write = tty_write,
};

void tty_task(void *arg) {
  device_t *in = dev_lookup("input");
  device_t *ttydev = dev_lookup("tty1");
  device_t *fb = dev_lookup("fb");

  tty_render(ttydev->ptr);
  while (1) {
    struct input_event ev;
    int nread = in->ops->read(in, 0, &ev, sizeof(ev));
    if (nread > 0) {
      if (ev.alt) {
        device_t *next = ttydev;
        if (ev.data == '1') next = dev_lookup("tty1");
        if (ev.data == '2') next = dev_lookup("tty2");
        if (ev.data == '3') next = dev_lookup("tty3");
        if (ev.data == '4') next = dev_lookup("tty4");
        if (ev.data == '7') printf("Switch to terminal\n");
        if (next != ttydev) {
          ttydev = next;
          tty_t *tty = ttydev->ptr;

          struct display_info info = {
            .current = tty->display,
          };
          tty_mark_all(tty);
          fb->ops->write(fb, 0, &info, sizeof(struct display_info));
          ttydev->ops->write(ttydev, 0, "", 0);
        }
      }
      if (ev.ctrl) {
        if (ev.data == 'c') printf("Ctrl - c\n");
      }
      if (!ev.ctrl && !ev.alt) {
        char ch = ev.data;
        tty_t *tty = ttydev->ptr;
        if (tty_cook(tty, ch) == 0) {
          ttydev->ops->write(ttydev, 0, &ch, 1);
        }
      }
    } else {
      panic("error");
    }
  }
}
