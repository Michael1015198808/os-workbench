#include "klib.h"
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
  while (1) {
    intptr_t value = atomic_xchg(locked, 1);
    if (value == 0) {
      break;
    }
  }
}
void pthread_mutex_unlock(pthread_mutex_t* locked) {
  atomic_xchg(locked, 0);
}
