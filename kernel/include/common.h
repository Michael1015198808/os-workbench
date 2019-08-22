#ifndef __COMMON_H__
#define __COMMON_H__

#include <kernel.h>
#include <nanos.h>

struct task {};
struct spinlock {};
struct semaphore {};

#define Assert(cond) \
    do{ \
        if(!(cond)){ \
            printf("%s:%d Assertion %s failed!\n",__FILE__,__LINE__, #cond); \
            _halt(1); \
        } \
    }while(0)
#endif
