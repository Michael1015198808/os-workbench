#include <devices.h>
#include <klib.h>
#include <yls.h>
#include <dir.h>

const char *warn="Haven't realized!\n";

static inline void cat_from_stdin(char buf[0x200],int* err){
    int nread=0;
    while((nread=vfs->read(STDIN,buf,0x200-1))>0){
        buf[nread]='\0';
        std_write(buf);
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
                do{
                    nread=vfs->read(fd,buf,sizeof(buf));
                    vfs->write(STDOUT,buf,nread);
                }while(nread>0);
                if(nread==EISDIR){
                    warn("%s: Is a directory",args[i]);
                }
            }else{
                cat_from_stdin(buf,&err);
            }
        }
    }else{
        cat_from_stdin(buf,&err);
    }
    return err;
}
