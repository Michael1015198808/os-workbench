#ifndef NO_TEST

typedef void (*test_func)(void*);
void (memory_test)(void*);
test_func p=memory_test;
test_func printf_test;
test_func spin_test;
test_func semaphore_test;
test_func multithread_test;
test_func fib_test;

#endif
