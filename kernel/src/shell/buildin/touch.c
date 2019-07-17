#include <devices.h>
#include <klib.h>
#include <yls.h>
#include <dir.h>

int mysh_touch(void *args[]){
    const char* pwd=get_pwd();
    if(args[1]){
        for(int i=1;args[i];++i){
            char file[0x100];
            to_absolute(file,pwd,args[i]);

            int fd=vfs->open(file,O_RDONLY|O_CREAT);
            if(fd<0){warn("Can not touch %s",args[i]);}
            vfs->close(fd);
        }
    }else{
        warning(" missing file operand");
    }
    return 0;
}
