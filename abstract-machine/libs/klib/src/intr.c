#include "klib.h"

#include "../am/src/x86/x86-qemu.h"
//Only for the macro MAX_CPU

int ncli[MAX_CPU]={},ori[MAX_CPU]={};

int intr_idx=0;
pthread_mutex_t intr_lk=PTHREAD_MUTEX_INITIALIZER;
char intr_log_string[66000];
pthread_mutex_t log_lk=PTHREAD_MUTEX_INITIALIZER;
void _intr_close(){
    int cpu_id=_cpu();
    Assert(ncli[cpu_id]>=0);
    ori[cpu_id]|=_intr_read();
    _intr_write(0);
    ++ncli[cpu_id];
}
void _intr_open(){
    int cpu_id=_cpu();
    --ncli[cpu_id];
    if(ncli[cpu_id]==0){
        _intr_write(ori[cpu_id]);
        ori[cpu_id]=0;
    }
    Assert(ncli[cpu_id]>=0);
}
/*
void intr_close(){
    _intr_close();
}
void intr_open(){
    _intr_open();
}
*/
