#include <devices.h>
#include <klib.h>
#include <vfs.h>
#include <yls.h>
#include <dir.h>

int mysh_rm(void *args[]){
    for(int i=1;args[i];++i){
        vfs->unlink(args[i]);
        error_print(" cannot remove '%s': ",args[i]);
    }
    return 0;
}

