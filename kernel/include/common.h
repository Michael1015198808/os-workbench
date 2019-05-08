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
struct task {};
struct spinlock {
uint32_t reentry_cnt,owner_pid;
pthread_mutex_t locked;
uint8_t int_on;
};
struct semaphore {};

#define log(fmt,...) printf("[cpu%d]" fmt, _cpu(),__VA_ARGS__)

#endif
