#include <common.h>
#include <klib.h>

static uintptr_t pm_start, pm_end;

#define KB *(1<<10)
struct header{
    struct header *next;
    uintptr_t size;
    struct{}space;//space doesn't take any storage
    //directly return &space
}static free_list[4]={};//Sentinels
typedef struct header header;
#define align(_A,_B) (_A+=(_B)-(_A&((_B)-1)))
#define PG_SIZE (8 KB)

uint16_t pages[(1<<12)+1]={};
void *bias;
void show_free_pages(void){
  int i;
  for(i=1;i<(1<<12);++i){
      printf("%4d:%2x\n",i,pages[i]);
  }
}
void enable(int idx,uintptr_t shift){
    if(idx==0)return;
    pages[idx]|=1<<shift;
    if(pages[idx>>1]&(1<<shift)){
        if(pages[idx^1]&(1<<shift)){
            enable(idx>>1,shift+1);
        }
    }else{
        enable(idx>>1,shift);
    }
}
void disable(int idx,uintptr_t shift){
    if(idx==0)return;
    pages[idx]&=~(1<<shift);
    if(pages[idx>>1]&(1<<(shift+1))){
         disable(idx>>1,shift+1);
    }else{
        if(!(pages[idx^1]&(1<<shift))){
            disable(idx>>1,shift);
        }
    }
}
static pthread_mutex_t alloc_lock=PTHREAD_MUTEX_INITIALIZER;
static void* big_page_alloc(uintptr_t shift){
    pthread_mutex_lock(&alloc_lock);
    int idx=1,level=12;
    while(--level!=shift){
        int left=idx<<1,right=left+1;
        if(pages[left]&(1<<shift)){
            idx=left;
        }else if(pages[right]&(1<<shift)){
            idx=right;
        }else{
            printf("No space left!\n");
            while(1);
            return NULL;//No space
        }
    }
    disable(idx,shift);
    pthread_mutex_unlock(&alloc_lock);
    return bias+
        ((idx<<shift)&((1<<11)-1))*PG_SIZE;
}
static void big_page_free(header *s){
    int idx=2047+
    (((uintptr_t)s)-((uintptr_t)bias))
    /PG_SIZE;
    while(s->size>PG_SIZE){
        enable(++idx,0);
        s->size-=PG_SIZE;
    }
}
static void pmm_init() {
  int i,cpu_cnt=_ncpu();
  pm_start = (uintptr_t)_heap.start;
  align(pm_start,PG_SIZE);
  pm_end   = (uintptr_t)_heap.end;
  bias=(void*)pm_start;
  for(i=0;i<cpu_cnt;++i){
      free_list[i].next=(void*)(pm_start+i*PG_SIZE);
      free_list[i].size=0;
      free_list[i].next->next=&free_list[i];
      free_list[i].next->size=PG_SIZE -sizeof(header);
  }
  for(i=cpu_cnt;i<(pm_end-pm_start)/(PG_SIZE)&&i<(1<<11);++i){
    enable((1<<11)+i,0);
  }
}

static void *kalloc(size_t size) {
  align(size,0x10);
  int cpu_id=_cpu();//Call once
  uint8_t *tail=NULL;
  header *p=free_list[cpu_id].next,*prevp=&free_list[cpu_id],*ret;
  if(size> PG_SIZE/2){
      size+=sizeof(header);
      int shift=0;
      size/=PG_SIZE;
      while(size>0){
          size>>=1;
          ++shift;
      }
      header *ret=big_page_alloc(shift);
      ret->size=size;
      return &(ret->space);
  }else{
    do{
      if(p->size>=size){
        if(p->size-size>sizeof(header)){
          tail=(uint8_t*)p;
          tail+=p->size;
          tail-=size;//Get to the tail
          ret=(header*)tail;
          ret->size=size;//record size for free
          p->size-=size+sizeof(header);//Shrink current space
          return &(ret->space);
        }else{
          prevp->next=p->next;//"delete" p
          return &(p->space);
        }
      }
      prevp=p;
      p=p->next;
    }while(p!=&free_list[cpu_id]);
  }
  prevp->next=big_page_alloc(0);//ask for a new page
  prevp->next->next=p;
  prevp->next->size=PG_SIZE-sizeof(header);
  return kalloc(size);
}

static void kfree(void *ptr) {
  if(ptr==NULL)return;
  int cpu_id=_cpu();//Call once
  header *p=free_list[cpu_id].next,
         *prevp=&free_list[cpu_id],
         *to_free=(header*)(ptr-sizeof(header));
      printf("%x\n",to_free->size);
  if(to_free->size> PG_SIZE/2){
    big_page_free(to_free);
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
void show_free_list(void){
    int cpu_id=_cpu();
    header *p=&free_list[cpu_id];
    printf("Free list:\n");
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
  .alloc = kalloc,
  .free = kfree,
};
