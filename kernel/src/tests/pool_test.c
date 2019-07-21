#include <devices.h>
#include <common.h>
#include <klib.h>

#ifndef NO_TEST
pool p[2];
void hello1(void *arg){
    void* get;
    while(1){
        enqueue(&p[0],arg);
        do{
            get=dequeue(&p[1]);
        }while(!get);
        _putc(*(char*)get);
    }
}
void hello2(void *arg){
    void* get;
    while(1){
        do{
            get=dequeue(&p[0]);
        }while(!get);
        _putc(*(char*)get);
        enqueue(&p[1],arg);
    }
}
void queue_test_init(void){
    kmt->create(pmm->alloc(sizeof(task_t)), "hello1", hello1, (void*)'a');
    kmt->create(pmm->alloc(sizeof(task_t)), "hello2", hello2, (void*)'b');
}
#endif

