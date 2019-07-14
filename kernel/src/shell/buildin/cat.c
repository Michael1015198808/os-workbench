#include <devices.h>
#include <klib.h>
#include <yls.h>
#include <dir.h>

const char *warn="Haven't realized!\n";

static void single_cat(int fd,char buf[0x208],int* err){
    int nread=0;
    while((nread=vfs->read(fd,buf,0x200))>0){
        vfs->write(STDOUT,buf,nread);
    }
    if(nread==EISDIR)*err=EISDIR;
    else std_write("\n");
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
                int fd=vfs->open(file,O_RDONLY);
                single_cat(fd,buf,&err);
                if(err==EISDIR){
                    warn("%s: Is a directory",args[i]);
                    err=0;
                }
            }else{
                single_cat(0,buf,&err);
            }
        }
    }else{
        single_cat(0,buf,&err);
    }
    return err;
}
