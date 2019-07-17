#include <devices.h>
#include <klib.h>
#include <yls.h>
#include <dir.h>

static void single_cat(int fd,char buf[0x208],char* file){
    int nread=0;
    while((nread=vfs->read(fd,buf,0x200))>0){
        vfs->write(STDOUT,buf,nread);
    }
    if(nread==EISDIR){
        warn("%s: Is a directory",file);
    }
}

int mysh_cat(void *args[]){
    const char* pwd=get_pwd();
    char buf[0x208];
    int err=0;
    if(args[1]){
        for(int i=1;args[i];++i){
            if(strcmp(args[i],"-")){
                char file[0x100];
                to_absolute(file,pwd,args[i]);

                if(vfs->access(file,O_RDONLY)==ENOENT){
                    fprintf(2,"%s: No such file or directory",file);
                }
                int fd=vfs->open(file,O_RDONLY);
                single_cat(fd,buf,file);
            }else{
                single_cat(0,buf,NULL);
            }
        }
    }else{
        single_cat(0,buf,NULL);
    }
    return err;
}
