#include <devices.h>
#include <common.h>
#include <klib.h>

#ifndef NO_TEST

void spin_test_init(void){
    kmt->create(pmm->alloc(sizeof(task_t)), "spin_test", spin_test, (void*)'a');
    kmt->create(pmm->alloc(sizeof(task_t)), "spin_test", spin_test, (void*)'b');
    kmt->create(pmm->alloc(sizeof(task_t)), "spin_test", spin_test, (void*)'c');
    kmt->create(pmm->alloc(sizeof(task_t)), "spin_test", spin_test, (void*)'d');
}
void spin_test(void *arg){
    char c=((char*)arg)[0];
    static spinlock_t global_lk;
    spinlock_t local_lk;
    while(1){
        printf("%c1%d\n",c,_intr_read());
        kmt->spin_lock(&global_lk);
        for(int i=0;i<5;++i)
            kmt->spin_lock(&local_lk);
        printf("%c0%d\n",c,_intr_read());
        for(int i=0;i<5;++i)
            kmt->spin_unlock(&local_lk);
        printf("%c0%d\n",c,_intr_read());
        kmt->spin_unlock(&global_lk);
    }
}

#endif
