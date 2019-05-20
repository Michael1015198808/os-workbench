#ifndef __COMMON_H__
#define __COMMON_H__

#include <kernel.h>
#include <klib.h>
#include <nanos.h>
#include <limits.h>
#include <stdint.h>

//#define sem_log(A,info,...) A->idx&=(1<<16)-1;A->idx+=sprintf(A->log+A->idx,"\n[cpu%d]%s:%d %s:%s %d",_cpu(),__func__,__LINE__,tasks[currents[_cpu()]]->name,  #info, A->value)

#define spinlock_log

#ifndef sem_log
    #define sem_log(...) 
#endif

#define intr_log(info) {int cpu_id=_cpu();(intr_idx_+=sprintf(intr_log_+intr_idx_,"\n [cpu%d]%s:%d %s(%d):%s %d",cpu_id,__FILE__,__LINE__,tasks[currents[cpu_id]]->name,currents[cpu_id],info,ncli[cpu_id]));intr_idx_[cpu_id]&=(1<<16)-1;}
#define detail_log(_log,_idx,info) \
    do{ \
        int cpu_id=_cpu(),_old=_idx; \
        _idx+=sprintf(_log+_idx,"\n[cpu%d]%s:%3d(%s) %s:%s",_cpu(),__FILE__,__LINE__,__func__,tasks[current]->name,info); \
        (void)_old; \
        tasks_old=_old; \
        /*printf(_log+_old);*/ \
        _idx&=(1<<16)-1; \
    }while(0)

#ifdef intr_log
    char intr_log_[66000];
    int intr_idx_;
#elif
    #define intr_log 
#endif

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
    uint32_t reen;
    int owner;
    pthread_mutex_t locked;
    char *name;
    uint8_t int_on;
#ifdef spinlock_log
    int idx;
    char log[66000];
#endif
}spinlock_t;

typedef struct List{
    task_t* task;
    struct List* next;
}list_t;
struct semaphore {
    char *name;
    volatile int value,capa;
    spinlock_t lock;
    list_t *head,*tail;
#ifdef sem_log
    int idx;
    char log[66000];
#endif
};
#define LEN(arr) ((sizeof(arr) / sizeof(arr[0])))

#define log(fmt,...) printf("[cpu%d]%s %3d:\n    " fmt,_cpu(),__FILE__,__LINE__,##__VA_ARGS__)
#define Assert(cond,...) \
    do { \
      if(!(cond)){ \
        printf("\33[1;31m"); \
        log("Assertion " #cond " fails!\n" __VA_ARGS__); \
        printf("\33[0m"); \
        _halt(0); \
      } \
    }while(0)

#define new(A) (typeof(A)*)pmm->alloc(sizeof(A))
#endif
