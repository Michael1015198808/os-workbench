#include <devices.h>
#include <common.h>
#include <klib.h>

semaphore_t test_sem[2];
//static volatile int to_run=1;
void semaphore_test_init(void){
    kmt->sem_init(&test_sem[0],"test sem:0",0);
    kmt->sem_init(&test_sem[1],"test sem:1",1);
}
void semaphore_test(void *arg){
    char c=((char*)arg)[0];
    int idx=c-'a';
    while(1){
        //while(to_run!=idx);
        kmt->sem_wait(  &test_sem[idx]);
        printf("[cpu%d]%d%cHello!\n",_cpu(),_intr_read(),c);
        //to_run^=1;
        kmt->sem_signal(&test_sem[1-idx]);
    }
}
