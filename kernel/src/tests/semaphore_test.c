#include <devices.h>
#include <common.h>
#include <klib.h>

semaphore_t test_sem[2];
void semaphore_test_init(void){
    kmt->sem_init(test_sem,"test sem:0",0);
    kmt->sem_init(&test_sem[1],"test sem:1",1);
}
void semaphore_test(void *arg){
    char c=((char*)arg)[0];
    int idx=c-'a';
    while(1){
        kmt->sem_wait(  &test_sem[idx]);
        _putc(c);
        for(int i=0;i<7;++i){
            _putc("Hello!\n"[i]);
        }
        kmt->sem_signal(&test_sem[1-idx]);
    }
}
