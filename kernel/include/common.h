#ifndef __COMMON_H__
#define __COMMON_H__

#include <kernel.h>
#include <klib.h>
#include <nanos.h>
#include <limits.h>
#include <stdint.h>
#include <macros.h>

#ifndef MAX_CPU
    #define MAX_CPU 8
#endif
#define NO_TEST
#include <test.h>

#include <my_trace.h>

#define STK_SZ ((1<<12)-64)
volatile int ncli[MAX_CPU],intena[MAX_CPU];

#define error_print(FMT, ...) \
    do{ \
        task_t* cur=get_cur(); \
        if(cur->err[0]){ \
            fprintf(2,"%s: " FMT "%s\n",cur->name, ##__VA_ARGS__,cur->err); \
            cur->err[0]='\0'; \
        } \
    }while(0)

typedef void(*task_fun)(void*);

inline task_t* get_cur(void){
    _intr_close();
    int cpu_id=_cpu();
extern task_t* currents[MAX_CPU];
    task_t* ret=currents[cpu_id];
    _intr_open();
    return ret;
}

int fprintf(int fd, const char *fmt, ...);
void warning(const char*);
void exit(void);

typedef struct device device_t;
void tty_set_color(device_t* dev,uint32_t* colors);
void tty_get_color(device_t* dev,uint32_t* colors);

typedef struct vfile vfile_t;
typedef struct inode inode_t;

#define TASK_RUNABLE    0
#define TASK_SLEEP      1
#define TASK_RUNNING    2
#define TASK_ZOMBIE     4
#define TASK_NOWAIT     8
#define FD_NUM 16
typedef struct task{
    //int32_t id;
    volatile uint32_t attr,ncli,intena,pid;
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
    vfile_t* fd[FD_NUM];
    inode_t* cur_dir;
    char pwd[0x100];
    char err[0x100];
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
typedef struct{
    pthread_mutex_t lk;
    void* mem[POOL_LEN];
    volatile int head,tail;
}pool;
void* dequeue(pool* p);
void enqueue(pool* p,void* mem);

typedef struct semaphore {
    char *name;
    volatile int value;
    pthread_mutex_t lock;
    task_t *pool[POOL_LEN];
    volatile int head,tail;
}semaphore_t;
#endif

void mysh(void*);
