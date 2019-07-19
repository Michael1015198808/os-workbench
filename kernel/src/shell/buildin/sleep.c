#include <devices.h>
#include <klib.h>
#include <yls.h>
#include <dir.h>
#include <vfs.h>

const char *warn="Haven't realized!\n";

static void single_sleep(void* arg){
    uint32_t begin=uptime();
    uint32_t to_sleep=0;
    const char* num=arg;
    while(*num){
        to_sleep*=10;
        to_sleep+=*num-'0';
        ++num;
    }
    to_sleep*=1000;
    while(uptime()-begin<to_sleep)_yield();
}

int mysh_sleep(void *args[]){
    if(args[1]){
        for(int i=1;args[i];++i){
            single_sleep(args[i]);
        }
    }else{
        fprintf(2,"sleep: missing operand\n");
        fprintf(2,"Usage: sleep NUMBER");
        exit();
    }
    return 0;
}

