#include <devices.h>
#include <common.h>
#include <klib.h>

#ifndef NO_TEST
pool p[3]={};
void queue1(void *arg){
    void* get;
    while(1){
        enqueue(&p[0],arg);
        do{
            get=dequeue(&p[1]);
        }while(!get);
        _putc(*(char*)get);
    }
}
void queue2(void *arg){
    void* get;
    while(1){
        do{
            get=dequeue(&p[0]);
        }while(!get);
        _putc(*(char*)get);
        enqueue(&p[1],arg);
    }
}

void pusher(void* arg){
    while(1){
        for(int i=0;i<5;++i){
            enqueue(p+2,arg+i);
        }
    }
}
void receiver(void* arg){
    char* get;
    int a=0,A=0;
    while(1){
        do{
            get=dequeue(p+2);
        }while(!get);
        _putc(*get);
        if(*get-'a'<5&&*get-'a'>=0){
            Assert('a'+a==*get,"Test failed!");
            ++a;
            if(a==5)a=0;
        }else{
            Assert('A'+A==*get,"Test failed!");
            ++A;
            if(A==5)A=0;
        }
    }
}
void queue_test_init(void){
    kmt->create(pmm->alloc(sizeof(task_t)),"hello1"  ,queue1  ,"("    );
    kmt->create(pmm->alloc(sizeof(task_t)),"hello2"  ,queue2  ,")"    );
    kmt->create(pmm->alloc(sizeof(task_t)),"pusher1" ,pusher  ,"ABCDE");
    kmt->create(pmm->alloc(sizeof(task_t)),"pusher2" ,pusher  ,"abcde");
    kmt->create(pmm->alloc(sizeof(task_t)),"receiver",receiver,NULL   );
}
#endif

