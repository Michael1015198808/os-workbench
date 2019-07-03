#include <devices.h>
#include <common.h>
#include <klib.h>

#ifndef NO_TEST
semaphore_t test_sem[4];
char *args[4]={"1","2","3","4"};
void semaphore_test_init(void){
    char sem_name[15]="test sem:";
    for(int i=0;i<LEN(test_sem);++i){
        sprintf(&sem_name[8],"%d",i);
        kmt->sem_init(&test_sem[i],sem_name,0);
        kmt->create(pmm->alloc(sizeof(task_t)),"semaphore_test",semaphore_test,args[i]);
    }
    kmt->sem_signal(&test_sem[0]);
}
void semaphore_test(void *arg){
    char c=((char*)arg)[0];
    int idx=c-'1';
    while(1){
        kmt->sem_wait(  &test_sem[idx]);
        Assert(_intr_read()==1,"ncli%d",ncli[_cpu()]);
        int next=rand()&3;
        printf("[cpu%d]%c->%dHello!\n",_cpu(),c,next);
        kmt->sem_signal(&test_sem[]);
    }
}

#endif
