#include <devices.h>
#include <klib.h>
#include <vfs.h>
#include <yls.h>
#include <dir.h>

static inline void single_cat_real(int fd,char buf[0x208],char* file){
    int nread=0;
    while((nread=vfs->read(fd,buf,0x200))>0){
        vfs->write(STDOUT,buf,nread);
    }
    if(nread==EISDIR){
        warn("Is a directory");
    }
}
static void single_cat(int fd,char buf[0x208],char* file){
    if(fd>=0){
        single_cat_real(fd,buf,file);
    }
    error_print("%s: ",file);
}

int mysh_cat(void *args[]){
    char buf[0x208];
    int err=0;
    if(args[1]){
        for(int i=1;args[i];++i){
            if(strcmp(args[i],"-")){
                int fd=vfs->open(args[i],O_RDONLY);
                single_cat(fd,buf,args[i]);
            }else{
                single_cat(0,buf,NULL);
            }
        }
    }else{
        single_cat(0,buf,NULL);
    }
    return err;
}
