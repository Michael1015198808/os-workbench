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
    func_t func;
    void *arg;
    jmp_buf tar_buf;
#define CO_ALIVE 1
#define CO_RUNNING 2
    uint8_t stat;
}routines[MAX_ROUTINES] /*__attribute((aligned(16)))*/,*current;

static void run_co()
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
    routines[pool[idx]].stat=CO_ALIVE;
    return &routines[pool[idx]];
}
void co_init() {
    int i;
    for(i=0;i<MAX_ROUTINES;++i){
        pool[i]=i;
        routines[i].stat=0;
    }
}

void *__stack_backup=NULL;
static jmp_buf ret_buf;
struct co* co_start(const char *name, func_t func, void *arg) {
  current=new_co();
  uint8_t* stack_top=current->stack+STACK_SIZE;
  stack_top-=sizeof(void*)+((void*)&name)-__stack_backup;
  //Calculate the space for entry parameters
#define mov_between(_para) \
  *(uintptr_t*)(stack_top+(((void*)&_para)-__stack_backup))=(uintptr_t)_para;
  mov_between(name);
  mov_between(func);
  mov_between(arg);

  current->stack_top=stack_top;
  current->func=func;
  current->arg=arg;
  longjmp(ret_buf,1);
  return current;
}

#define run_co(_co) \
  current=_co; \
  if(!(_co->stat&&CO_ALIVE)) \
    fprintf(stderr,"Run a dead coroutine!\n");fflush(stderr); \
  if(_co->stat&&CO_RUNNING){ \
    longjmp(_co->tar_buf,1); \
  }else{ \
    _co->stat|=CO_RUNNING; \
    set_sp(_co->stack_top); \
    _co->func(_co->arg) \
  }
void co_yield() {
    int val=setjmp(current->tar_buf);
    if(val==0){
        longjmp(ret_buf,1);
    }else{
        return;
    }
}

void co_wait(struct co *thd) {
  get_sp(__stack_backup);
  setjmp(ret_buf);
  while(thd->alive){
      run_co(thd);
  }
  set_sp(__stack_backup);
  thd->stat=0;
}

