#include <devices.h>
#include <common.h>
#include <klib.h>

#ifndef NO_TEST
void hello1(void *arg){
    while(1){
        asm volatile("nop");
        printf("Hello%c\n",(char)(uintptr_t)arg);
    }
}
void hello2(void *arg){
    while(1){
        printf("Hello%c\n",(char)(uintptr_t)arg);
        asm volatile("nop");
    }
}
void context_test_init(void){
    kmt->create(pmm->alloc(sizeof(task_t)), "hello1[1]", hello1, (void*)'a');
    kmt->create(pmm->alloc(sizeof(task_t)), "hello2[1]", hello2, (void*)'b');
    kmt->create(pmm->alloc(sizeof(task_t)), "hello1[2]", hello1, (void*)'c');
    kmt->create(pmm->alloc(sizeof(task_t)), "hello2[2]", hello2, (void*)'d');
}
#endif
