#include <devices.h>
#include <klib.h>
#include <yls.h>
#include <dir.h>
#include <vfs.h>

int mysh_test(void *args[]){
    uint32_t begin=uptime();
    for(int i=1;i<=20;++i){
        while(uptime()-begin<1000)_yield();
        begin=uptime();
        fprintf(1,"Hello, world for the %d-th time!\n",i);
    }
    return 0;
}
