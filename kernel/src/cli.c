#include <klib.h>
#include <common.h>

int ncli[4]={},ori[4]={};
void intr_close(){
    int cpu_id=_cpu();
    Assert(ncli[cpu_id]>=0);
    if(ncli[cpu_id]==0){
        ori[cpu_id]=_intr_read();
        _intr_write(0);
    }
    ++ncli[cpu_id];
}
void intr_open(){
    int cpu_id=_cpu();
    --ncli[cpu_id];
    if(ncli[cpu_id]==0){
        _intr_write(ori[cpu_id]);
    }
    Assert(ncli[cpu_id]>=0);
}
