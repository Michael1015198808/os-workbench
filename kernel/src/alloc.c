#include <common.h>
#include <klib.h>

static uintptr_t pm_start, pm_end;

#define KB *(1<<10)
struct header{
    struct header *next;
    uintptr_t size;
    struct{}space;//space doesn't take any storage
    //directly return &space
}static *free_list[4]={};
typedef struct header header;

static void pmm_init() {
  int i,cpu_cnt=_ncpu();
  pm_start = (uintptr_t)_heap.start;
  pm_start = -pm_start;
  pm_start&= -((uintptr_t)0x10);
  pm_start = -pm_start;
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
          assert(0);
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
          printf("%p\n",ret);
          ret->size=size;//record size for free
          p->size-=size;//Shrink current space
          printf("%p\n",&(ret->space));
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
  int cpu_id=_cpu();//Call once
  header *p=free_list[cpu_id]->next,
         *prevp=free_list[cpu_id],
         *to_free=(header*)(ptr-sizeof(header));
  if(to_free->size> 1 KB){
    //TODO: fancy algorithm
  }
  while((uintptr_t)ptr>(uintptr_t)&(p->space)&&(p->next!=free_list[cpu_id])){
    prevp=p;
    p=p->next;
  }
  printf("free:%p\nheader:%p\n",ptr,to_free);
  //*prevp---*to_free---*p
  if(((uintptr_t)to_free)==((uintptr_t)prevp)+prevp->size){
    prevp->size+=sizeof(header)+to_free->size;
    if(((uintptr_t)to_free)+to_free->size!=((uintptr_t)prevp)+prevp->size){
        printf("to_free%p,len:%x\n prevp%p,len:%x\n",to_free,to_free->size,prevp,prevp->size);
    }
    to_free=prevp;//Merge to_free with prevp
  }else{
    to_free->next=p->next;
    p->next=to_free;
  }
  if(((uintptr_t)p)==((uintptr_t)to_free)+to_free->size){
    to_free->next=p->next;
    to_free->size+=sizeof(header)+p->size;
  }
}
void show_free_list(void){
    int cpu_id=_cpu();
    header *p=free_list[cpu_id];
    printf("Free list:\n");
    do{
        printf("[%p,%p):%x\n",p,((void*)p)+p->size,p->size);
        p=p->next;
    }while(p!=free_list[cpu_id]);
    printf("\n");
}

MODULE_DEF(pmm) {
  .init = pmm_init,
  .alloc = kalloc,
  .free = kfree,
};
