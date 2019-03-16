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
    uintptr_t args_space[2];
    //compiler will allocate space as I declare
    //Use args_space[idx] to make assignment easier
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
    if(
            (void*)routines[0].args_space!=
            (void*)(routines[0].stack+STACK_SIZE)){
        fprintf(stderr,"The compiler doesn't allocate space as it should do\n");
        fprintf(stderr,"Top of stack%p\n",routines[0].args_space);
        fprintf(stderr,"Entry para %p\n",routines[0].args_space);
        fflush(stderr);
        exit(1);
    }
    int i;
    for(i=0;i<MAX_ROUTINES;++i){
        pool[i]=i;
        routines[i].alive=0;
    }
}

static void *__stack_backup=NULL;
static jmp_buf ret_buf;
struct co* co_start(const char *name, func_t func, void *arg) {
  get_sp(__stack_backup);
  current=new_co();
  //asm volatile("mov 0x0("SP"),%0;": "=g"(current->args_space[0]):);
  current->args_space[0]=func;
  //asm volatile("mov %1("SP"),%0;": "=g"(current->args_space[1]):"n"(sizeof(void*)));
  current->args_space[1]=arg;
  set_sp(current->args_space);
  if(!setjmp(ret_buf)){
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
}

