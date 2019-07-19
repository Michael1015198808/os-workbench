#include <klib.h>
#include <devices.h>
#include <vfs.h>

int mysh_mkdir(void *args[]){
    if(args[1]){
        for(int i=1;args[i];++i){
            vfs->mkdir(args[i]);
        }
        return 0;
    }else{
        fprintf(2, "Usage: mkdir files...\n");
        return -1;
    }
}

