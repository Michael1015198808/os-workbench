#include <devices.h>
#include <common.h>
#include <klib.h>

#ifndef NO_TEST
pool p1;
void hello1(void *arg){
    void* get;
    enqueue(&p1,arg);
    while(1){
        do{
            get=dequeue(&p2);
        }while(!get);
        _putc(*(char*)get);
        enqueue(&p1,arg);
    }
}
void hello2(void *arg){
    void* get;
    while(1){
        do{
            get=dequeue(&p1);
        }while(!get);
        _putc(*(char*)get);
        enqueue(&p2,arg);
    }
}
void queue_test_init(void){
    kmt->create(pmm->alloc(sizeof(task_t)), "hello1", hello1, (void*)'a');
    kmt->create(pmm->alloc(sizeof(task_t)), "hello2", hello2, (void*)'b');
}
#endif

