#include <kernel.h>
#include <klib.h>

MODULE_DEF(kmt) {
  .init=kmt_init,
  .create=kmt_create,
  .teardown=kmt_teardown,
  .spin_init=kmt_spin_init,
  .spin_lock=kmt_spin_lock,
  .spin_unlock=kmt_spin_unlock,
  .sem_init=kmt_sem_init,
  .sem_wait=kmt_sem_wait,
  .sem_signal=kmt_sem_signal
}
void kmt_init(void){
    return;
}
int kmt_create(task_t *task, const char *name, void (*entry)(void *arg), void *arg){
    return 0;
}
void kmt_teardown(task_t *task){
    return ;
}
void kmt_spin_init(spinlock_t *lk, const char *name){
}
void kmt_spin_lock(spinlock_t *lk){

}
void kmt_spin_unlock(spinlock_t *lk){

}
void kmt_sem_init(sem_t *sem, const char *name, int value){

}
void kmt_sem_wait(sem_t *sem){

}
void kmt_sem_signal(sem_t *sem){

}
