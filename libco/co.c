#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <setjmp.h>
#include "co.h"

#define log() printf("Line %d:\n",__LINE__)
#if defined(__i386__)
  #define SP "%%esp"
  #define AX "%%eax"
#elif defined(__x86_64__)
  #define SP "%%rsp"
  #define AX "%%rax"
#endif
#define set_sp(__target) asm volatile("mov %0," SP : : "g"(__target));
#define get_sp(__target) asm volatile("mov " SP",%0" : "=g"(__target) :);
#define MAX_ROUTINES 100

struct co {
#define KB *(1<<10)
#define STACK_SIZE (4 KB)
    uint8_t stack[STACK_SIZE];
    void * stack_top;
    //stack should provide room for entry parameters
    jmp_buf tar_buf;
    uint8_t alive;
}routines[MAX_ROUTINES] /*__attribute((aligned(16)))*/,*current;

static int pool[MAX_ROUTINES];
//pool[0,idx) records indexes of
//available space for routines.
struct co* new_co(){
    static int idx=MAX_ROUTINES;
    if(idx==0){
        fflush(stdout);
        fprintf(stderr,"pool overflow!\n");
        fflush(stderr);
    }
    --idx;
    routines[pool[idx]].alive=1;
    return &routines[pool[idx]];
}
void co_init() {
    int i;
    for(i=0;i<MAX_ROUTINES;++i){
        pool[i]=i;
        routines[i].alive=0;
    }
}

void *__stack_backup=NULL;
static jmp_buf ret_buf;
struct co* co_start(const char *name, func_t func, void *arg) {
  get_sp(__stack_backup);
  if(!setjmp(ret_buf)){
    current=new_co();
    uint8_t* stack_top=current->stack+STACK_SIZE;
    stack_top-=sizeof(void*)+((void*)&name)-__stack_backup;
    *(uintptr_t*)(stack_top+(((void*)&name)-__stack_backup))=(uintptr_t)name;
    *(uintptr_t*)(stack_top+(((void*)&func)-__stack_backup))=(uintptr_t)func;
    *(uintptr_t*)(stack_top+(((void*)&arg )-__stack_backup))=(uintptr_t)arg;
    current->stack_top=stack_top;
    set_sp(stack_top);
    func(arg);
  }
  set_sp(__stack_backup);
  return current;
}

void co_yield() {
    int val=setjmp(current->tar_buf);
    if(val==0){
        set_sp(__stack_backup);
        longjmp(ret_buf,1);
    }else{
        return;
    }
}

void co_wait(struct co *thd) {
    while(thd->alive){
        set_sp(thd->stack_top);
        longjmp(thd->tar_buf,1);
    }
}

