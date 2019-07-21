#include <devices.h>
#include <klib.h>
#include <vfs.h>
#include <yls.h>
#include <dir.h>

int mysh_link(void *args[]){
    if(args[1]&&args[2]){
        if(args[3]){
            fprintf(2,"To much operand\nUsage: link FILE1 FILE2\n");
            return -1;
        }
        get_cur()->err[0]='\0';
        int ret=vfs->link(args[1],args[2]);
        error_print("cannot create link '%s' to '%s': ",args[2],args[1]);
        return ret;
    }else{
        fprintf(2,"Missing operand\nUsage: link FILE1 FILE2\n");
    }
    return -1;
}
