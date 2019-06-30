#include <devices.h>
#include <common.h>
#include <klib.h>
void spin_test(void *dummy){
    char c=((char*)dummy)[0];
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
        kmt->spin_unlock(&global_lk);
    }
}
