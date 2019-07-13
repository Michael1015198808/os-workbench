#include <devices.h>
#include <klib.h>
#include <yls.h>
#include <dir.h>

const char *warn="Haven't realized!\n";

static void single_cat(int fd,char buf[0x200],int* err){
    int nread=0;
    while((nread=vfs->read(fd,buf,0x200-1))>0){
        buf[nread]='\0';
        std_write(buf);
    }
    if(nread==EISDIR){
        warn("%s: Is a directory",args[i]);
    }
    if(nread<0)*err=-1;
}

int mysh_cat(void *args[]){
    const char* pwd=get_pwd();
    char buf[0x200];
    int err=0;
    if(args[1]){
        for(int i=1;args[i];++i){
            if(strcmp(args[i],"-")){
                char file[0x100];
                to_absolute(file,pwd,args[i]);
                int fd=vfs->open(file,O_RDONLY),nread=0;
                single_cat(fd,buf,&err);
            }else{
                single_cat(0,buf,&err);
            }
        }
    }else{
        single_cat(0,buf,&err);
    }
    return err;
}
