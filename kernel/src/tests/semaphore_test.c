#include <devices.h>
#include <common.h>
#include <klib.h>

#ifndef NO_TEST
semaphore_t test_sem[4];
char args[]={"0123"};
void semaphore_test_init(void){
    char sem_name[15]="test sem:";
    for(int i=0;i<LEN(test_sem);++i){
        sprintf(&sem_name[8],"%d",i);
        kmt->sem_init(&test_sem[i],sem_name,0);
        kmt->create(pmm->alloc(sizeof(task_t)),"semaphore_test",semaphore_test,args+i);
    }
    kmt->sem_signal(&test_sem[0]);
}
#undef Assert
#define Assert(...) 
void semaphore_test(void *arg){
    char c=((char*)arg)[0];
    int idx=c-'0';
    while(1){
        Assert(_intr_read()==1,"ncli%d",ncli[_cpu()]);
        kmt->sem_wait(  &test_sem[idx]);
        Assert(_intr_read()==1,"ncli%d",ncli[_cpu()]);
        int next=(idx+1)&3;
        extern task_t *tasks[0x40];
        printf("[cpu%d]%c->%dHello!(%d%d%d%d)\n",_cpu(),c,next,tasks[0]->running,tasks[1]->running,tasks[2]->running,tasks[3]->running);
        Assert(_intr_read()==1,"ncli%d",ncli[_cpu()]);
        Assert(ncli[_cpu()]==0,"ncli%d",ncli[_cpu()]);
        kmt->sem_signal(&test_sem[next]);
        Assert(_intr_read()==1,"ncli%d",ncli[_cpu()]);
    }
}

#endif
