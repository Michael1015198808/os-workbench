#include <devices.h>
#include <common.h>
#include <klib.h>

#define semaphore_test_init \
    void semaphore_test_init(void); \
    semaphore_test_init();
void semaphore_test_init(void){
}
semaphore_t test_sem;
void semaphore_test(void *arg){
}
