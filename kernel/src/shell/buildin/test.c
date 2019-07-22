#include <devices.h>
#include <klib.h>
#include <yls.h>
#include <dir.h>
#include <vfs.h>

int mysh_test(void *args[]){
    int tty=isatty(STDOUT);
    if(tty){
        std_write("is a tty\n");
    }
void tty_set_color(device_t* dev,uint32_t fg,uint32_t bg);
    tty_set_color(get_cur()->fd[1]->inode->ptr,0x00000000,0x00ffffff);
    if(tty){
        std_write("is a tty\n");
    }
    tty_set_color(get_cur()->fd[1]->inode->ptr,0x00ffffff,0x00000000);
    /*
    uint32_t begin=uptime();
    for(int i=1;i<=20;++i){
        while(uptime()-begin<1000)_yield();
        begin=uptime();
        fprintf(1,"Hello, world for the %d-th time!\n",i);
    }
    */
    return 0;
}
