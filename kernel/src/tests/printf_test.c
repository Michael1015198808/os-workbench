#include <common.h>
#include <klib.h>

#ifndef NO_TEST
void printf_test_init(void){
    kmt->create(pmm->alloc(sizeof(task_t)),"printf_test1",printf_test,"a");
    kmt->create(pmm->alloc(sizeof(task_t)),"printf_test2",printf_test,"b");
    kmt->create(pmm->alloc(sizeof(task_t)),"printf_test3",printf_test,"c");
    kmt->create(pmm->alloc(sizeof(task_t)),"printf_test4",printf_test,"d");
    kmt->create(pmm->alloc(sizeof(task_t)),"printf_test5",printf_test,"e");
}
void printf_test(void *dummy){
    while(1){
        printf("Hello!%c\n",((char*)dummy)[0]);
    }
}

#endif
