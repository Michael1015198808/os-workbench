#ifndef __KERNEL_H__
#define __KERNEL_H__

#include <am.h>
#include <nanos.h>

#define MODULE(name) \
  mod_##name##_t; \
  extern mod_##name##_t *name
#define MODULE_DEF(name) \
  extern mod_##name##_t __##name##_obj; \
  mod_##name##_t *name = &__##name##_obj; \
  mod_##name##_t __##name##_obj = 

typedef _Context *(*handler_t)(_Event, _Context *);
typedef struct {
  void (*init)();
  void (*run)();
  _Context *(*trap)(_Event ev, _Context *context);
  void (*on_irq)(int seq, int event, handler_t handler);
} MODULE(os);

typedef struct {
  void (*init)();
  void *(*alloc)(size_t size);
  void (*free)(void *ptr);
} MODULE(pmm);

typedef struct task task_t;
typedef struct spinlock spinlock_t;
typedef struct semaphore sem_t;
typedef struct {
  void (*init)();
  int (*create)(task_t *task, const char *name, void (*entry)(void *arg), void *arg);
  void (*teardown)(task_t *task);
  void (*spin_init)(spinlock_t *lk, const char *name);
  void (*spin_lock)(spinlock_t *lk);
  void (*spin_unlock)(spinlock_t *lk);
  void (*sem_init)(sem_t *sem, const char *name, int value);
  void (*sem_wait)(sem_t *sem);
  void (*sem_signal)(sem_t *sem);
} MODULE(kmt);

typedef struct device device_t;
typedef struct devops {
  int (*init)(device_t *dev);
  ssize_t (*read)(device_t *dev, off_t offset, void *buf, size_t count);
  ssize_t (*write)(device_t *dev, off_t offset, const void *buf, size_t count);
} devops_t;
typedef struct {
  void (*init)();
} MODULE(dev);

#endif
