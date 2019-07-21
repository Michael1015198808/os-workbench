#include <devices.h>
#include <klib.h>
#include <vfs.h>
#include <yls.h>
#include <dir.h>

int mysh_unlink(void *args[]){
    if(args[1]){
        if(args[2]){
            fprintf(2,"To much operand\nUsage: unlink FILE\n");
            return -1;
        }
        int ret=vfs->unlink(args[1]);
        if(ret){
            printf("unlink failed!\n");
        }
        error_print(" cannot unlink '%s': ",args[1]);
        return ret;
    }else{
        fprintf(2,"Missing operand\nUsage: unlink FILE\n");
    }
    return -1;
}
