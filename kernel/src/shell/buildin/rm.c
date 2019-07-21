#include <devices.h>
#include <klib.h>
#include <vfs.h>
#include <yls.h>
#include <dir.h>

int mysh_unlink(void *args[]){
    for(int i=1;args[i];++i){
        vfs->unlink(args[i]);
    }
    return 0;
}

