#include <devices.h>
#include <common.h>
#include <klib.h>

#ifndef NO_TEST
void hello1(void *arg){
    while(1){
        asm volatile("nop");
        printf("Hello%c\n",*(char*)arg);
    }
}
void hello2(void *arg){
    while(1){
        printf("Hello%c\n",*(char*)arg);
        asm volatile("nop");
    }
}
contest_test_init(){
    kmt->create(pmm->alloc(sizeof(task_t)), "hello1[1]", hello1, 'a');
    kmt->create(pmm->alloc(sizeof(task_t)), "hello2[1]", hello2, 'b');
    kmt->create(pmm->alloc(sizeof(task_t)), "hello1[2]", hello1, 'c');
    kmt->create(pmm->alloc(sizeof(task_t)), "hello2[2]", hello2, 'd');
}
#endif
