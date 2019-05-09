#ifndef __COMMON_H__
#define __COMMON_H__

#include <kernel.h>
#include <nanos.h>

#define DEBUG
//Comment the line above after testing
#ifdef DEBUG
void show_free_list(void);
uintptr_t cnt_free_list(void);
void show_free_pages(void);
#endif
#define STK_SZ ((1<<12)-64)
typedef struct{
    int32_t id;
    char* name;
    uint32_t fence1[4];
    uint8_t stack[STK_SZ];
    uint32_t fence2[4];
}task_t;
typedef struct{
    uint32_t reen,owner;
    pthread_mutex_t locked;
    char *name;
    uint8_t int_on;
}spinlock_t;
struct semaphore {
    char *name;
    int value;
    spinlock_t lock;
    //TODO: 嘤嘤嘤
};

#define log(fmt,...) printf("%s:%3d[cpu%d]" fmt,__FILE__,__LINE__,_cpu(),__VA_ARGS__)
#define assert(cond,...) \
    do { \
      if(!cond){ \
        log(__VA_ARGS__); \
        halt(0); \
      } \
    }while(0)

#define new(A) (typeof(A)*)malloc(pmm->kalloc(A))
#endif
