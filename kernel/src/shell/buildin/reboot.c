#include <klib.h>
#include <devices.h>

int mysh_reboot(void *args[]){
    asm volatile("jmp 0x0");
    Assert(0,"Should not reach here!");
}

