#include <stdio.h>
#include "co.h"

struct co {
};

void co_init() {
}

struct co* co_start(const char *name, func_t func, void *arg) {
  func(arg); // Test #2 hangs
  return NULL;
}

void co_yield() {
}

void co_wait(struct co *thd) {
}

