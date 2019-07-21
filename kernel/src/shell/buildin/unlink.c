#include <devices.h>
#include <klib.h>
#include <vfs.h>
#include <yls.h>
#include <dir.h>

int mysh_unlink(void *args[]){
    if(args[1]&&!args[2]){
        return vfs->unlink(args[1]);
    }else{
        fprintf(2,"Missing operand\nUsage: unlink FILE\n");
    }
    return -1;
}

