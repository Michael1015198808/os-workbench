#ifndef __COMMON_H__
#define __COMMON_H__

#include <kernel.h>
#include <klib.h>
#include <nanos.h>
#include <limits.h>
#include <stdint.h>

#include <vfs.h>

#ifndef MAX_CPU
    #define MAX_CPU 8
#endif
//#define NO_TEST
#include <test.h>

#include <my_trace.h>

#define TODO Assert(0,"Haven't be realized!\n");
#define MACRO_VALUE(_arg)     #_arg
#define MACRO_SELF(_arg)      _arg
#define MACRO_CONCAT_REAL(_arg1,_arg2) _arg1 ## _arg2
#define MACRO_CONCAT(_arg1,_arg2) MACRO_CONCAT_REAL(_arg1,_arg2)
#define TO_STRING(_arg)     MACRO_VALUE(_arg)

#define local_log(...) 

#define LEN(arr) ((sizeof(arr) / sizeof(arr[0])))

#define new(A) (typeof(A)*)pmm->alloc(sizeof(A))

#define TASK_FENCE

#define STK_SZ ((1<<12)-64)
volatile int ncli[MAX_CPU],intena[MAX_CPU];

#define TASK_RUNABLE 0
#define TASK_SLEEP 1
#define TASK_RUNNING 2
#define FD_NUM 16
typedef struct task{
    //int32_t id;
    volatile uint32_t attr,ncli,intena,cpu;
    pthread_mutex_t running;
    char* name;
    _Context context;
#ifdef TASK_FENCE
    uint32_t fence1[4];
#endif
    uint8_t stack[STK_SZ];
    struct{}stack_end;
#ifdef TASK_FENCE
    uint32_t fence2[4];
#endif
    vfile_t fd[FD_NUM];
}task_t;
extern task_t* currents[MAX_CPU];

typedef struct spinlock{
    uint32_t reen;
    task_t* owner;
    pthread_mutex_t locked;
    char *name;
#ifdef spinlock_log
    int idx;
    char log[66000];
#endif
}spinlock_t;

typedef struct List{
    task_t* task;
    struct List* next;
}list_t;

#define POOL_LEN 20
typedef struct semaphore {
    char *name;
    volatile int value;
    pthread_mutex_t lock;
    task_t *pool[POOL_LEN];
    volatile int head,tail;
}semaphore_t;
#endif

void report_if(int);
void mysh(void*);
