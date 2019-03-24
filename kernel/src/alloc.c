#include <common.h>
#include <klib.h>

static uintptr_t pm_start, pm_end;

#define KB *(1<<10)
struct header{
    struct header *next;
    unsigned size;
}*free_list[4]={};

static void pmm_init() {
  printf("ncpu:%d\n",_ncpu());
  int i;
  pm_start = (uintptr_t)_heap.start;
  pm_end   = (uintptr_t)_heap.end;
  for(i=0;i<ncpu();++i){
      free_list[i]=pm_start;
      free_list[i]->next=&free_list[i];
      free_list[i]->size=1KB-sizeof(struct header);
      pm_start+=1KB;
  }
}

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
