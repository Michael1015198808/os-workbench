#include <dir.h>
#include <klib.h>
#include <devices.h>
#include <vfs.h>

int mysh_rmdir(void *args[]){
    if(args[1]){
        for(int i=1;args[i];++i){
            vfs->rmdir(args[i]);
            error_print(" cannot remove '%s': ",args[i]);
        }
        return 0;
    }else{
        fprintf(2,"Usage: rmdir [FILE]\n");
        return -1;
    }
    return 0;
}
