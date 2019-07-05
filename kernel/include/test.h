#ifndef __TEST_H
#define __TEST_H

#ifndef NO_TEST

typedef void (test_func)(void*);
test_func memory_test;
test_func printf_test;
test_func spin_test;
test_func semaphore_test;
test_func multithread_test;
test_func fib_test;
test_func context_test;

#endif//NO_TEST

#endif//__TEST_H
