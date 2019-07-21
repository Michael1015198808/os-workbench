#include <devices.h>
#include <klib.h>
#include <vfs.h>
#include <yls.h>
#include <dir.h>

int mysh_link(void *args[]){
    char buf[0x208];
    int err=0;
    if(args[1]&&args[2]&&!args[3]){
        vfs->link(args[1],args[2]);
    }else{
        fprintf(2,"Missing operand\nUsage: link FILE1 FILE2\n");
    }
}
