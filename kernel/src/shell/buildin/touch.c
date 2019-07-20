#include <devices.h>
#include <klib.h>
#include <yls.h>
#include <dir.h>
#include <vfs.h>

int mysh_touch(void *args[]){
    if(args[1]){
        for(int i=1;args[i];++i){
            int fd=vfs->open(args[i],O_WRONLY|O_APPEND|O_CREAT);
            if(fd<0){warn("Can not touch %s",args[i]);}
            vfs->close(fd);
        }
    }else{
        warning(" missing file operand");
    }
    return 0;
}
