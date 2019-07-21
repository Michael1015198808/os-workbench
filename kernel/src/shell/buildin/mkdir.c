#include <devices.h>
#include <klib.h>
#include <yls.h>
#include <dir.h>
#include <vfs.h>

int mysh_mkdir(void *args[]){
    if(args[1]){
        for(int i=1;args[i];++i){
            clear_warn();
            if(vfs->mkdir(args[i])){
                error_print("");
            }
        }
        return 0;
    }else{
        fprintf(2, "Usage: mkdir files...\n");
        return -1;
    }
}

