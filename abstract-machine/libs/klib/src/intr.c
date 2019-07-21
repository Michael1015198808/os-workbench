#include "klib.h"

#include "../am/src/x86/x86-qemu.h"
//Only for the macro MAX_CPU

//volatile int ncli[MAX_CPU]={},intena[MAX_CPU]={};
volatile int ncli[MAX_CPU]={},intena[MAX_CPU]={};

volatile int intr_idx=0;
pthread_mutex_t intr_lk=PTHREAD_MUTEX_INITIALIZER;
char intr_log_string[66000];
pthread_mutex_t log_lk=PTHREAD_MUTEX_INITIALIZER;
void _intr_close(){
    int int_on=_intr_read();
    cli();
    int cpu_id=_cpu();
    Assert(ncli[cpu_id]>=0,"Int count is wrong!");
    intena[cpu_id]|=int_on;
    ++ncli[cpu_id];
}
void _intr_open(){
    int cpu_id=_cpu();
    --ncli[cpu_id];
    if(ncli[cpu_id]==0){
        if(intena[cpu_id]){
            intena[cpu_id]=0;
            sti();
        }
    }
    Assert(ncli[cpu_id]>=0,"Int is already open!");
}
/*
void intr_close(){
    _intr_close();
}
void intr_open(){
    _intr_open();
}
*/
