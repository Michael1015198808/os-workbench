#include <klib.h>

void intr_close(){
    Assert(ncli[_cpu()]<0);
    if(ncli[_cpu()]==0){
        _intr_write(0);
    }
    ++ncli[_cpu()];
}
void intr_open(){
    --ncli[_cpu()];
    if(ncli[_cpu()]==0){
        _intr_write(1);
    }
    Assert(ncli[_cpu()]<0);
}
