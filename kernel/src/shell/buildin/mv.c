#include <devices.h>
#include <shell.h>
#include <klib.h>
#include <vfs.h>
#include <yls.h>
#include <dir.h>

int mysh_mv(void *args[]){
    if(args[1]&&args[2]){
        if(args[3]){
            fprintf(2,"To much operand\nUsage: mv SOURCE DEST\n  or:  mv FILE DIRECTORY\n");
            return -1;
        }
        int ret=0;
        if(( ret=mysh_cp(args)     ))return ret;
        args[2]=NULL;//To pass parameter check of unlink
        if(( ret=mysh_unlink(args) ))return ret;
        return 0;
    }else{
        fprintf(2,"Missing operand\nUsage: mv SOURCE DEST\n  or:  mv FILE DIRECTORY\n");
    }
    return -1;
}


