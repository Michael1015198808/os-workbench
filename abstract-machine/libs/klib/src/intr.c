#include "klib.h"

#include "../am/src/x86/x86-qemu.h"
//Only for the macro MAX_CPU

int ncli[MAX_CPU]={},ori[MAX_CPU]={};
volatile int idx=0;
char intr_log_string[66000];
pthread_mutex_t log_lk=PTHREAD_MUTEX_INITIALIZER;
int intr_idx_=0;
static inline void _intr_close(){
    int cpu_id=_cpu();
    Assert(ncli[cpu_id]>=0);
    ori[cpu_id]|=_intr_read();
    _intr_write(0);
    ++ncli[cpu_id];
}
static inline void _intr_open(){
    int cpu_id=_cpu();
    --ncli[cpu_id];
    if(ncli[cpu_id]==0){
        _intr_write(ori[cpu_id]);
        ori[cpu_id]=0;
    }
    Assert(ncli[cpu_id]>=0);
}
void intr_log(char *s){
#define LOG(...) idx+=sprintf(intr_log_string+idx,__VA_ARGS__)
    pthread_mutex_t lk=PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&lk);
    LOG(s);
    LOG("[%d,%d]",ncli[0],ncli[1]);
    pthread_mutex_unlock(&lk);
#undef LOG
}
/*
void intr_close(){
    _intr_close();
}
void intr_open(){
    _intr_open();
}
*/
