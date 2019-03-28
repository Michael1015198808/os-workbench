#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <setjmp.h>
#include "co.h"

#define log() printf("Line %d:\n",__LINE__)
#if defined(__i386__)
  #define SP "%%esp"
#elif defined(__x86_64__)
  #define SP "%%rsp"
#endif
#define set_sp(__target) asm volatile("mov %0," SP : : "g"(__target));
#define get_sp(__target) asm volatile("mov " SP",%0" : "=g"(__target) :);
#define MAX_ROUTINES 100

struct co {
#define KB *(1<<10)
#define STACK_SIZE (4 KB)
    uint8_t stack[STACK_SIZE];
    //stack should provide room for entry parameters
    jmp_buf tar_buf;
#define CO_ALIVE (1<<0)
    uint8_t stat;
}routines[MAX_ROUTINES] __attribute__((aligned(16))),*current;

static int pool[MAX_ROUTINES];
//pool[0,idx) records indexes of
//available space for routines.
static int pool_idx=0;
struct co* new_co(){
    if(pool_idx<=0){
        fflush(stdout);
        fprintf(stderr,"pool overflow!\n");
        fflush(stderr);
    }
    --pool_idx;
    routines[pool[pool_idx]].stat=CO_ALIVE;
    return &routines[pool[pool_idx]];
}
void co_init() {
    int i;
    pool_idx=MAX_ROUTINES;
    for(i=0;i<MAX_ROUTINES;++i){
        pool[i]=i;
        routines[i].stat=0;
    }
}

void *__stack_backup=NULL;
static jmp_buf ret_buf;
struct co* co_start(const char *name, func_t func, void *arg) {
  get_sp(__stack_backup);
  current=new_co();
  uint8_t*
      stack_top=current->stack
                +STACK_SIZE
                -STACK_SIZE/4;
  //Space for entry parameters and other things
  //I used &name-__stack_backup to get the extra space, but failed
#define mov_to(_para,_stack) \
  *(uintptr_t*)(_stack+ \
      /* bias*/(((void*)&_para)-__stack_backup) \
          )=(uintptr_t)_para;
  //Move parameters to the new stack
  mov_to(name,stack_top);
  mov_to(func,stack_top);
  mov_to(arg,stack_top);

  set_sp(stack_top);
  if(!setjmp(current->tar_buf)){
    set_sp(__stack_backup);
    return current;
  }else{
    func(arg);
    current->stat&=~CO_ALIVE;
    pool[pool_idx++]=current-routines;
    longjmp(ret_buf,1);
  }
  return NULL;//will not reach here
}


void co_yield() {
    if(!setjmp(current->tar_buf)){
        int next_co;
        do{
            next_co=rand()%MAX_ROUTINES;
        }while(!(routines[next_co].stat&CO_ALIVE));
        current=&routines[next_co];
        longjmp(routines[next_co].tar_buf,1);
    }
}

void co_wait(struct co *thd) {
  if(!setjmp(ret_buf)){
    while(thd->stat&CO_ALIVE){
      current=thd;
      longjmp(thd->tar_buf,1); 
    }
  }
  thd->stat=0;
}

