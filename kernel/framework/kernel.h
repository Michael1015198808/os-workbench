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

#endif
