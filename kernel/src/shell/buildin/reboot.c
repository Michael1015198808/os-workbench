#include <klib.h>
#include <devices.h>

int mysh_reboot(void *args[]){
    asm volatile("jmp 0xfff0");
    Assert(0,"Should not reach here!");
}

