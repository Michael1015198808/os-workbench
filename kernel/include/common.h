#ifndef __COMMON_H__
#define __COMMON_H__

#include <kernel.h>
#include <klib.h>
#include <nanos.h>
#include <limits.h>
#include <stdint.h>

#define sem_log(A,info) A.idx+=sprintf(A->log+A->idx,"[cpu%d]%s:%s\n",_cpu(),tasks[currents[_cpu()]]->name, #info)
#define TASK_FENCE
#define DEBUG
//Comment the line above after testing
#ifdef DEBUG
void show_free_list(void);
uintptr_t cnt_free_list(void);
void show_free_pages(void);
#endif
#define STK_SZ ((1<<12)-64)
int ncli[4];
void intr_close();
void intr_open();

typedef struct task{
    //int32_t id;
    int32_t cpu;
    char* name;
    _Context context;
    //enum state{SLEEPING,WAITING,RUNNING}state;
#ifdef TASK_FENCE
    uint32_t fence1[4];
#endif
    uint8_t stack[STK_SZ];
    struct{
    }stack_end;
#ifdef TASK_FENCE
    uint32_t fence2[4];
#endif
}task_t;

typedef struct spinlock{
    uint32_t reen,owner;
    pthread_mutex_t locked;
    char *name;
    uint8_t int_on;
}spinlock_t;

typedef struct List{
    task_t* task;
    struct List* next;
}list_t;
struct semaphore {
    char *name;
    int value,capa;
    spinlock_t lock;
    list_t *head,*tail;
#ifdef sem_log
    int idx;
    char log[5000];
#endif
};
#define LEN(arr) ((sizeof(arr) / sizeof(arr[0])))

#define log(fmt,...) printf("[cpu%d]%s %3d:\n    " fmt,_cpu(),__FILE__,__LINE__,##__VA_ARGS__)
#define Assert(cond,...) \
    do { \
      if(!(cond)){ \
        log("\33[1;31mAssertion " #cond " fails!\33[0m\n" __VA_ARGS__); \
        _halt(0); \
      } \
    }while(0)

#define new(A) (typeof(A)*)pmm->alloc(sizeof(A))
#endif
