#include <devices.h>

#define NEVENTS 128
sem_t sem_kbdirq;

static struct input_event event(int ctrl, int alt, int data) {
  return (struct input_event) {
    .ctrl = ctrl,
    .alt = alt,
    .data = data,
  };
}

static void push_event(input_t *in, struct input_event ev) {
  kmt->spin_lock(&in->lock);
  in->events[in->rear] = ev;
  in->rear = (in->rear + 1) % NEVENTS;
  if (in->rear == in->front) { panic("input queue full"); }
  kmt->spin_unlock(&in->lock);
  kmt->sem_signal(&in->event_sem);
}

static struct input_event pop_event(input_t *in) {
  kmt->sem_wait(&in->event_sem);
  kmt->spin_lock(&in->lock);
  if (in->rear == in->front) { panic("input queue empty"); }
  int idx = in->front;
  in->front = (in->front + 1) % NEVENTS;
  struct input_event ret = in->events[idx];
  kmt->spin_unlock(&in->lock);
  return ret;
}

void input_keydown(device_t *dev, int code) {
  input_t *in = dev->ptr;
  int key = code & ~0x8000, ch;

  if (code & 0x8000) {
    // keydown
    switch (key) {
      case _KEY_CAPSLOCK: in->capslock     ^= 1; break; 
      case _KEY_LCTRL:    in->ctrl_down[0]  = 1; break; 
      case _KEY_RCTRL:    in->ctrl_down[1]  = 1; break; 
      case _KEY_LALT:     in->alt_down[0]   = 1; break; 
      case _KEY_RALT:     in->alt_down[1]   = 1; break; 
      case _KEY_LSHIFT:   in->shift_down[0] = 1; break; 
      case _KEY_RSHIFT:   in->shift_down[1] = 1; break; 
      default:
        ch = keymap[key];
        if (ch) {
          int shift = in->shift_down[0] || in->shift_down[1];
          int ctrl = in->ctrl_down[0] || in->ctrl_down[1];
          int alt = in->alt_down[0] || in->alt_down[1];

          if (ctrl || alt) {
            push_event(in, event(ctrl, alt, ch));
          } else {
            if (ch >= 'a' && ch <= 'z') {
              shift ^= in->capslock;
            }
            if (shift) {
              push_event(in, event(0, 0, keymap_shift[key]));
            } else {
              push_event(in, event(0, 0, ch));
            }
          }
        }
    }
  } else {
    // keyup
    switch (code) {
      case _KEY_LCTRL:  in->ctrl_down[0]  = 0; break; 
      case _KEY_RCTRL:  in->ctrl_down[1]  = 0; break; 
      case _KEY_LALT:   in->alt_down[0]   = 0; break; 
      case _KEY_RALT:   in->alt_down[1]   = 0; break; 
      case _KEY_LSHIFT: in->shift_down[0] = 0; break; 
      case _KEY_RSHIFT: in->shift_down[1] = 0; break; 
    }
  }
}

static _Context *input_notify(_Event ev, _Context *context) {
  kmt->sem_signal(&sem_kbdirq);
  return NULL;
}

void input_task(void *args) {
  device_t *in = dev_lookup("input");

  while (1) {
    int code;
    while ((code = read_key()) != 0) {
      input_keydown(in, code);
    }
    kmt->sem_wait(&sem_kbdirq);
  }
}

static int input_init(device_t *dev) {
  input_t *in = dev->ptr;
  in->events = pmm->alloc(sizeof(in->events[0]) * NEVENTS);
  in->front = in->rear = 0;
  kmt->spin_init(&in->lock, "/dev/input lock");
  kmt->sem_init(&in->event_sem, "events in queue", 0);
  kmt->sem_init(&sem_kbdirq, "keyboard-interrupt", 0);

  os->on_irq(0, _EVENT_IRQ_IODEV, input_notify);
  return 0;
}

static ssize_t input_read(device_t *dev, off_t offset, void *buf, size_t count) {
  struct input_event ev = pop_event(dev->ptr);
  if (count >= sizeof(ev)) {
    memcpy(buf, &ev, sizeof(ev));
    return sizeof(ev);
  } else {
    return 0;
  }
}

static ssize_t input_write(device_t *dev, off_t offset, const void *buf, size_t count) {
  return 0;
}

devops_t input_ops = {
  .init = input_init,
  .read = input_read,
  .write = input_write,
};
