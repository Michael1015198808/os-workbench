#include <devices.h>
#include <common.h>
#include <klib.h>
void spin_test(){
    static spinlock_t test_spin;
    static int i=0;
    if(i==0){
        kmt->spin_init(&test_spin,"test spin");
        i=1;
    }
    _intr_write(0);
    kmt->spin_lock(&test_spin);
    kmt->spin_lock(&test_spin);
    kmt->spin_unlock(&test_spin);
    kmt->spin_unlock(&test_spin);
    for(volatile int i=0;i<1000;++i);
    kmt->spin_lock(&test_spin);
    printf("cpu %d gets the final lock\n",_cpu());
    while(1);
}
