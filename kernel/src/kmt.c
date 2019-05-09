#include <common.h>
#include <klib.h>

#define copy_name(dest,src) \
    dest=pmm->alloc(strlen(src)+1); \
    memcpy(dest,src,strlen(src)+1);
void kmt_init(void){
    return;
}
int kmt_create(task_t *task, const char *name, void (*entry)(void *arg), void *arg){
    static int32_t id=0;
    task->id=id++;
    copy_name(task->name,name);
    return 0;
}
void kmt_teardown(task_t *task){
    pmm->kfree(task->name);
    return ;
}
void kmt_spin_init(spinlock_t *lk, const char *name){
    lk->locked=PTHREAD_MUTEX_INITIALIZER;
    copy_name(lk->name,name);
}
void kmt_spin_lock(spinlock_t *lk){
    static pthread_mutex_t inner_lock=PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&inner_lock);
    if(lk->locked&&lk->owner==_cpu()){
        ++lk->reen;
    }else{
        lk->int_on=_intr_read();
        intr_write(1);
        pthread_mutex_lock(&lk->locked);
        lk->reen=1;
        lk->owner=_cpu();
    }
    pthread_mutex_unlock(&inner_lock);
}
void kmt_spin_unlock(spinlock_t *lk){
    if(lk->locked){
        if(lk->owner!=_cpu()){
            log("Lock[%s] isn't holded by this CPU!\n",lk->name);
        }else{
            if(lk->reen==1){
                lk->owner=-1;
                intr_write(lk->int_on);
                //True but sometimes slow
                pthread_mutex_unlock(&(lk->locked));
            }else{
                --lk->reen;
            }
        }
    }else{
        log("Lock[%s] isn't locked!\n",lk->name);
        while(1);
    }
}
void kmt_sem_init(sem_t *sem, const char *name, int value){
    copy_name(sem->name,name);
    sem->value=value;
}
void kmt_sem_wait(sem_t *sem){
    pthread_mutex_lock(sem->lock);
    --(sem->value);
    while(sem->value==0){
        pthread_mutex_unlock(&(sem->lock));
        _yield();
        pthread_mutex_lock&((sem->lock));
    }
    pthread_mutex_unlock(sem->lock);
}
void kmt_sem_signal(sem_t *sem){
    ++(sem->value);
}
MODULE_DEF(kmt) {
  .init        =kmt_init,
  .create      =kmt_create,
  .teardown    =kmt_teardown,
  .spin_init   =kmt_spin_init,
  .spin_lock   =kmt_spin_lock,
  .spin_unlock =kmt_spin_unlock,
  .sem_init    =kmt_sem_init,
  .sem_wait    =kmt_sem_wait,
  .sem_signal  =kmt_sem_signal
};
