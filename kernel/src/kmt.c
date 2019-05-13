#include <common.h>
#include <klib.h>

#define copy_name(dest,src) \
    dest=pmm->alloc(strlen(src)+1); \
    memcpy(dest,src,strlen(src)+1);

static _Context* kmt_context_save(_Event ev, _Context *c){
    log("context_save\n");
    return NULL;
}
static _Context* kmt_context_switch(_Event ev, _Context *c){
    log("context_switch\n");
    return c;
}
void kmt_init(void){
    os->on_irq(INT_MIN, _EVENT_NULL, kmt_context_save);
    os->on_irq(INT_MAX, _EVENT_NULL, kmt_context_switch);
    void irq_test(void);
    irq_test();
}
int kmt_create(task_t *task, const char *name, void (*entry)(void *arg), void *arg){
    static int32_t id=0;
    task->id=id++;
    copy_name(task->name,name);
    
    task->context = *_kcontext((_Area){task->stack,&(task->stack_end)}, entry, arg);
#ifdef TASK_FENCE
    for(int i=0;i<4;++i){
        task->fence1[i]=0x13579ace;
        task->fence2[i]=0xeca97531;
    }
#endif
    return 0;
}
void kmt_teardown(task_t *task){
    pmm->free(task->name);
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
        _intr_write(1);
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
                _intr_write(lk->int_on);
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
    kmt->spin_lock(&(sem->lock));
    --(sem->value);
    while(sem->value==0){
        kmt->spin_lock(&(sem->lock));
        _yield();
        kmt->spin_lock(&(sem->lock));
    }
    kmt->spin_lock(&(sem->lock));
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
