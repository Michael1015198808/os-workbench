#include <common.h>
#include <klib.h>

static uintptr_t pm_start, pm_end;

static void pmm_init() {
  printf("pmm_init(%d)\n",_cpu());
  pm_start = (uintptr_t)_heap.start;
  pm_end   = (uintptr_t)_heap.end;
}

struct header{
    struct header *next;
    unsigned size;
}*free_list[4]={};
#define KB *(1<<10)
static void *kalloc(size_t size) {
  return NULL;
}

static void kfree(void *ptr) {
}

MODULE_DEF(pmm) {
  .init = pmm_init,
  .alloc = kalloc,
  .free = kfree,
};
