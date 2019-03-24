#include <common.h>
#include <klib.h>

static uintptr_t pm_start, pm_end;

#define KB *(1<<10)
struct header{
    struct header *next;
    uintptr_t size;
    uintptr_t space;//directly return &space
}*free_list[4]={};
typedef struct header header;

static void pmm_init() {
  int i,cpu_cnt=_ncpu();
  pm_start = (uintptr_t)_heap.start;
  pm_end   = (uintptr_t)_heap.end;
  for(i=0;i<cpu_cnt;++i){
      free_list[i]=(void*)pm_start;
      free_list[i]->next=(header*)&(free_list[i]->space);
      free_list[i]->size=0;//Sentinel
      header *head=free_list[i]->next;
      head->next=free_list[i];//Circular
      head->size=1 KB-sizeof(header)*2+sizeof(uintptr_t);
      pm_start+=1 KB;
      if(((void*)head)+head->size+sizeof(header)!=(void*)pm_start){
          printf("result%p\nactual%p\n",
                  ((void*)head)+head->size+sizeof(header),
                  (void*)pm_start);
      }
  }
}

static void *kalloc(size_t size) {
  int cpu_id=_cpu();//Call once
  uint8_t *tail=NULL;
  header *p=free_list[cpu_id],*prevp=NULL,*ret;
  do{
    if(p->size>=size){
      if(p->size-size>16){
        tail=(uint8_t*)p;
        tail+=p->size;
        tail-=size;//Get to the tail
        ret=(header*)tail;
        ret->size=size;//record size for free
        p->size-=size;//Shrink current space
        return &(ret->space);
      }else{
        return NULL;
      }
    }
    prevp=p;
    p=p->next;
  }while(p!=free_list[cpu_id]);
  //TODO: ask os for one more page
  //return kalloc(size);
  return prevp;//Prevent compile error
}

static void kfree(void *ptr) {
}

MODULE_DEF(pmm) {
  .init = pmm_init,
  .alloc = kalloc,
  .free = kfree,
};
