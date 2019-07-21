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
}static free_list[MAX_CPU]={},*global;//Sentinels
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
  global->size=pm_end-pm_start-sizeof(header);
}

header *global_alloc_real(size_t size){
    if(global->size>size+sizeof(header)){
        uint8_t* tail=(uint8_t*)global;
        tail+=global->size;
        tail-=size;
        header* ret=(header*)tail;
        global->size-=size+sizeof(header);
        return ret;
    }else{
        printf("No free space!\n");
        return NULL;
    }
}

header* global_alloc(size_t size){
    static pthread_mutex_t global_lk=PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&global_lk);
    header* ret=global_alloc_real(size);
    pthread_mutex_unlock(&global_lk);
    return ret;
}

static void *kalloc_real(size_t size) {
    align(size,0x10);
    int cpu_id=_cpu();//Call once
    uint8_t *tail=NULL;
    header *p=free_list[cpu_id].next,*prevp=&free_list[cpu_id],*ret;
    while(1){
        for(    p=free_list[cpu_id].next;
                p!=&free_list[cpu_id];
                prevp=p,
                p=p->next){

            if(p->size>=size+sizeof(header)){
                tail=(uint8_t*)p;
                tail+=p->size;
                tail-=size;//Get to the tail
                p->size-=size+sizeof(header);//Shrink current block
                ret=(header*)tail;
                ret->size=size;//record size for free
                return ret;
            }else if(p->size>=size){
                prevp->next=p->next;//"Remove" p
                return p;//from free list
            }
        }

        size_t needed=size>PG_SIZE?size:PG_SIZE;
        prevp->next=global_alloc(needed);//ask for a new page
        Assert(prevp->next!=NULL);
        prevp->next->next=p;
        prevp->next->size=needed;
        p=prevp;
    }
    Assert(0,"Should not reach here");
}

static uint32_t alloc_cnt=0;
static void *kalloc(size_t size){
    intr_close();
    header* p=kalloc_real(size);
    p->fence=0x13579ace;
    intr_open();
    alloc_cnt+=p->size;
    memset(&p->space,0,size);
    //printf("Return %x,%p\n",size,p);
    return &p->space;
}

static inline void kfree_real(void *ptr) {
    int cpu_id=_cpu();
    header *p=free_list[cpu_id].next,
            *prevp=&free_list[cpu_id],
            *to_free=(header*)(ptr-sizeof(header));
    if(to_free->fence!=0x13579ace){
        printf("Fence at %x changed to %x!\n",&to_free->fence,to_free->fence);
    }
    to_free->fence=0xeca97531;//Alert if multi-free
    for(  ;
            p!=&free_list[cpu_id];
            prevp=p,
            p=p->next){
        if((uintptr_t)ptr<(uintptr_t)&(p->space)){
            break;
        }
    }
    //prevp---to_free---p
    //    prevp-\  to_free
    //free_list-/
    if(((uintptr_t)to_free)==((uintptr_t)&prevp->space)+prevp->size){
        prevp->size+=sizeof(header)+to_free->size;
        to_free=prevp;//Merge to_free with prevp
    }else{
        to_free->next=prevp->next;
        prevp->next=to_free;
    }
    if(((uintptr_t)p)==((uintptr_t)&to_free->space)+to_free->size){
        //Merge if
        //to_free-----p
        //       <-0->
        to_free->next=p->next;
        to_free->size+=sizeof(header)+p->size;
    }
}

static uint32_t free_cnt=0;
static void kfree(void *ptr){
    if(ptr==NULL)return;
    free_cnt+=
        ((header*)(ptr-sizeof(header)))->size;
    kfree_real(ptr);
}

MODULE_DEF(pmm) {
  .init = pmm_init,
  .alloc = kalloc,
  .free = kfree,
};

void mem_query(uint32_t num[3]){
    num[0]=pm_end-pm_start;
    num[1]=alloc_cnt-free_cnt;
}
