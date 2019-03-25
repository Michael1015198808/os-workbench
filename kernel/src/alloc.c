#include <common.h>
#include <klib.h>

static uintptr_t pm_start, pm_end;

#define KB *(1<<10)
struct header{
    struct header *next;
    uintptr_t size;
    struct{}space;//space doesn't take any storage
    //directly return &space
}*free_list[4]={};
typedef struct header header;

static void pmm_init() {
  int i,cpu_cnt=_ncpu();
  pm_start = (uintptr_t)_heap.start;
  printf("%x\n",pm_start);
  pm_start = -pm_start;
  printf("%x\n",pm_start);
  pm_start&= -((uintptr_t)0x10);
  printf("%x\n",pm_start);
  pm_start = -pm_start;
  printf("%x\n",pm_start);
  pm_end   = (uintptr_t)_heap.end;
  for(i=0;i<cpu_cnt;++i){
      free_list[i]=(void*)pm_start;
      free_list[i]->next=(header*)&(free_list[i]->space);
      free_list[i]->size=0;//Sentinel
      header *head=free_list[i]->next;
      head->next=free_list[i];//Circular
      head->size=1 KB-sizeof(header)*2;
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
  header *p=free_list[cpu_id]->next,*prevp=free_list[cpu_id],*ret;
  if(size> 1 KB){
    static pthread_mutex_t kalloc_lock;
    pthread_mutex_lock(&kalloc_lock);
    //TODO:Fancy algorithm
    pthread_mutex_unlock(&kalloc_lock);
  }else{
    do{
      if(p->size>=size){
        if(p->size-size>sizeof(header)){
          tail=(uint8_t*)p;
          tail+=p->size;
          tail-=size;//Get to the tail
          ret=(header*)tail;
          ret->size=size;//record size for free
          p->size-=size;//Shrink current space
          return &(ret->space);
        }else{
          prevp->next=p->next;//"delete" p
          return &(p->space);
        }
      }
      prevp=p;
      p=p->next;
    }while(p!=free_list[cpu_id]);
  }
  return prevp;//Prevent compile error
}

static void kfree(void *ptr) {
}

MODULE_DEF(pmm) {
  .init = pmm_init,
  .alloc = kalloc,
  .free = kfree,
};
