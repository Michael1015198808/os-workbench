#include <common.h>
#include <klib.h>

static uintptr_t pm_start, pm_end;

#define KB *(1<<10)
struct header{
    struct header *next;
    uintptr_t size;
    uint32_t fence;
    __attribute__((aligned(0x10)))struct{}space;//space doesn't take any storage
    //directly return &space
}static free_list[4]={},*global;//Sentinels
typedef struct header header;
#define align(_A,_B) (_A=( (_A+_B-1)/_B)*_B)
#define PG_SIZE (8 KB)

static void pmm_init() {
  int i,cpu_cnt=_ncpu();
  pm_start = (uintptr_t)_heap.start;
  align(pm_start,PG_SIZE);
  pm_end   = (uintptr_t)_heap.end;

  for(i=0;i<cpu_cnt;++i){
      free_list[i].next=&free_list[i];
      free_list[i].size=0;
  }
  global=(void*)pm_start;
  global->next=global;
  global->size=pm_end-pm_start;

}
header *global_alloc_real(size_t size){
    if(global->size>size+sizeof(header)){
        tail=(uint8_t*)p;
        tail+=global->size;
        tail-=size;
        ret=(header*)tail;
        global->size-=size+sizeof(header);
        return ret;
    }else{
        printf("No free space!\n");
        report_if(1);
    }
}
header* global_alloc(size_t size){
    pthread_mutex_t global_lk=PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&global_lk);
    header* ret=global_alloc_real(size);
    pthread_mutex_unlock(&global_lk);
    return ret;
}

static void *kalloc(size_t size) {
    align(size,0x10);
    int cpu_id=_cpu();//Call once
    uint8_t *tail=NULL;
    header *p=free_list[cpu_id].next,*prevp=&free_list[cpu_id],*ret;
    do{
        do{
            if(p->size>=size){
                if(p->size-size>sizeof(header)){
                    tail=(uint8_t*)p;
                    tail+=p->size;
                    tail-=size;//Get to the tail
                    ret=(header*)tail;
                    ret->size=size;//record size for free
                    p->size-=size+sizeof(header);//Shrink current space
                    ret->fence=0x13579ace;
                    return &(ret->space);
                }else{
                    prevp->next=p->next;//"delete" p
                    p->fence=0x13579ace;
                    return &(p->space);
                }
            }
            prevp=p;
            p=p->next;
        }while(p!=&free_list[cpu_id]);

        prevp->next=global_alloc(size>PG_SIZE?size:PG_SIZE);//ask for a new page
        Assert(prevp->next!=NULL);
        prevp->next->next=p;
        prevp->next->size=PG_SIZE-sizeof(header);
    }while(1);
    Assert(0,"Should not reach here");
}
uint64_t alloc_cnt=0;
static void *wrap_kalloc(size_t size){
    alloc_cnt+=size;
    void* p=kalloc(size);
    memset(p,0,size);
    //printf("Return %x,%p\n",size,p);
    return p;
}

uint64_t free_cnt=0;
static inline void kfree(void *ptr) {
  if(ptr==NULL)return;
  int cpu_id=_cpu();//Call once
  header *p=free_list[cpu_id].next,
         *prevp=&free_list[cpu_id],
         *to_free=(header*)(ptr-sizeof(header));
  free_cnt+=to_free->size;
  if(to_free->fence!=0x13579ace){
    printf("Fence at %x changed to %x!\n",&to_free->fence,to_free->fence);
    report_if(1);
    //while(1);
  }
  while((uintptr_t)ptr>(uintptr_t)&(p->space)&&p!=&free_list[cpu_id]){
    prevp=p;
    p=p->next;
  }
  //*prevp---*to_free---*p
  if(((uintptr_t)to_free)==((uintptr_t)&prevp->space)+prevp->size){
    prevp->size+=sizeof(header)+to_free->size;
    to_free=prevp;//Merge to_free with prevp
  }else{
    to_free->next=prevp->next;
    prevp->next=to_free;
  }
  if(((uintptr_t)p)==((uintptr_t)&to_free->space)+to_free->size){
    to_free->next=p->next;
    to_free->size+=sizeof(header)+p->size;
  }
}
static void wrap_kfree(void *ptr){
    kfree(ptr);
}

void show_free_list(void){
    int cpu_id=_cpu();
    header *p=&free_list[cpu_id];
    printf("Free list of CPU:#%d:\n",cpu_id);
    do{
        printf("[%p,%p):%x\n",p,((void*)p)+p->size,p->size);
        p=p->next;
    }while(p!=&free_list[cpu_id]);
    printf("\n");
}
uintptr_t cnt_free_list(void){
    int cpu_id=_cpu();
    uintptr_t ret=0;
    header *p=&free_list[cpu_id];
    do{
        ret+=p->size;
        p=p->next;
    }while(p!=&free_list[cpu_id]);
    return ret;
}

MODULE_DEF(pmm) {
  .init = pmm_init,
  .alloc = wrap_kalloc,
  .free = wrap_kfree,
};
