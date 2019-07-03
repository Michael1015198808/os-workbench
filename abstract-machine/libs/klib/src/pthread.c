#include "klib.h"
#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

//#include <stdarg.h>
static intptr_t atomic_xchg(volatile pthread_mutex_t *addr,intptr_t newval) {
  // swap(*addr, newval);
  intptr_t result;
  asm volatile ("lock xchg %0, %1":
    "+m"(*addr), "=a"(result) : "1"(newval) : "cc");
  return result;
}
//TODO: handle reentrance
//TODO: handle waiting by maintaining a thread pool
void pthread_mutex_lock(pthread_mutex_t* locked) {
  while (atomic_xchg(locked, 1));
}
void pthread_mutex_unlock(pthread_mutex_t* locked) {
  atomic_xchg(locked, 0);
}
int pthread_mutex_trylock(pthread_mutex_t* locked) {
    return atomic_xchg(locked, 1);
}
#endif
