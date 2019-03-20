#include <common.h>
intptr_t atomic_xchg(volatile intptr_t *addr,
                               intptr_t newval) {
  // swap(*addr, newval);
  intptr_t result;
  asm volatile ("lock xchg %0, %1":
    "+m"(*addr), "=a"(result) : "1"(newval) : "cc");
  return result;
}
intptr_t locked = 0;
void lock(intptr_t locked) {
  while (1) {
    intptr_t value = atomic_xchg(&locked, 1);
    if (value == 0) {
      break;
    }
  }
}
void unlock(intptr_t locked) {
  atomic_xchg(&locked, 0);
}
