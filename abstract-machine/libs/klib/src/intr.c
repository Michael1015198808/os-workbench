#include "klib.h"

#include "../am/src/x86/x86-qemu.h"
//Only for the macro MAX_CPU

int ncli[MAX_CPU]={},ori[MAX_CPU]={};
char intr_log_[66000];
pthread_mutex_t log_lk=PTHREAD_MUTEX_INITIALIZER;
int intr_idx_=0;
void intr_close(){
    int cpu_id=_cpu();
    Assert(ncli[cpu_id]>=0);
    ori[cpu_id]|=_intr_read();
    _intr_write(0);
    ++ncli[cpu_id];
}
void intr_open(){
    int cpu_id=_cpu();
    --ncli[cpu_id];
    if(ncli[cpu_id]==0){
        _intr_write(ori[cpu_id]);
        ori[cpu_id]=0;
    }
    Assert(ncli[cpu_id]>=0);
}
