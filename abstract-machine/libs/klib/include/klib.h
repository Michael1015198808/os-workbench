/*
 * Static runtime library for a system software on AbstractMachine
 */

#ifndef __KLIB_H__
#define __KLIB_H__

#include <am.h>
#include <stdarg.h>

//#define __NATIVE_USE_KLIB__

#ifdef __NATIVE_USE_KLIB__
#define strlen my_strlen
#define strcpy my_strcpy
#define strncpy my_strncpy
#define strcat my_strcat
#define strcmp my_strcmp
#define strncmp my_strncmp
#define memset my_memset
#define memcpy my_memcpy
#define memcmp my_memcmp
#define printf my_printf
#define vsprintf my_vsprintf
#define sprintf my_sprintf
#define snprintf my_snprintf

#define pthread_mutex_t my_pthread_mutex_t
#define pthread_mutex_lock my_pthread_mutex_lock
#define pthread_mutex_unlock my_pthread_mutex_unlock
#define pthread_mutex_trylock my_pthread_mutex_trylock
#endif

#ifdef __cplusplus
extern "C" {
#endif

// am devices

uint32_t uptime();
void get_timeofday(void *rtc);
int read_key();
void draw_rect(uint32_t *pixels, int x, int y, int w, int h);
void draw_sync();
int screen_width();
int screen_height();

// string.h
size_t strlen   (const char* s);
size_t strnlen   (const char* s,size_t maxlen);
char* strcpy    (char* dst       ,const char* src);
char* strncpy   (char* dst       ,const char* src,size_t n);
char* strcat    (char* dst       ,const char* src);
char* strncat   (char* dst       ,const char* src,size_t n);
int strcmp      (const char* s1  ,const char* s2);
int strncmp     (const char* s1  ,const char* s2 ,size_t n);
void* memset    (void* v         ,int c          ,size_t n);
void* memcpy    (void* dst       ,const void* src,size_t n);
int memcmp      (const void* s1  ,const void* s2 ,size_t n);
void* memmove   (void* dst       ,const void* src,size_t n);
char* strchr    (const char *s   ,char c);

// stdlib.h
void srand(unsigned int seed);
int rand();
int atoi(const char*);

// stdio.h
int printf(const char* fmt, ...);
int sprintf(char* out, const char* format, ...);
int snprintf(char* s, size_t n, const char* format, ...);
int vsprintf(char *str, const char *format, va_list ap);

// pthread.h
typedef volatile uintptr_t pthread_mutex_t;
#define PTHREAD_MUTEX_INITIALIZER 0
void pthread_mutex_lock(pthread_mutex_t* locked);
void pthread_mutex_unlock(pthread_mutex_t* locked);
int pthread_mutex_trylock(pthread_mutex_t* locked);

// assert.h
#ifdef NDEBUG
  #define assert(ignore) ((void)0)
#else
  #define assert(cond) \
    do { \
      if (!(cond)) { \
        printf("Assertion fail at %s:%d\n", __FILE__, __LINE__); \
        _halt(1); \
      } \
    } while (0)
#endif

#ifdef __cplusplus
}
#endif


//Below is my functions.
//intr.c
void _intr_close();
void _intr_open();

#define log(fmt,...) printf("[cpu%d]%s %3d:\n    " fmt,_cpu(),__FILE__,__LINE__,##__VA_ARGS__)
#define Assert(cond,...) \
    do { \
      if(!(cond)){ \
        asm volatile("nop;nop;"); \
/* Make it easier to set breakpoints.*/\
        printf("\33[1;31m"); \
        log("Assertion " #cond " fails!\n" __VA_ARGS__); \
        printf("\33[0m"); \
        _halt(0); \
      } \
    }while(0)

#define intr_close() \
    do{ \
        intr_log("[cpu%d]%s:%d(%s)close\n",_cpu(),__FILE__,__LINE__,__func__); \
        _intr_close(); \
    }while(0)
#define intr_open() \
    do{ \
        intr_log("[cpu%d]%s:%d(%s)open\n" ,_cpu(),__FILE__,__LINE__,__func__); \
        _intr_open(); \
    }while(0)

volatile int intr_idx;
pthread_mutex_t intr_lk;
#define intr_log(...) /*\
    pthread_mutex_lock(&intr_lk); \
    LOG("[%d,%d]\n",ncli[0],ncli[1]); \
    LOG(__VA_ARGS__); \
    pthread_mutex_unlock(&intr_lk);*/

#endif
//__KLIB_H__
