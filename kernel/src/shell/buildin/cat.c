#include <klib.h>
#include <devices.h>
const char *warn="Haven't realized!\n";

static void cat_from_stdin(void){
    int nread=0;
    while((nread=vfs->read(STDIN,buf,sizeof(buf)))){
        if(nread==0)return 0;
        buf[nread]='\0';
        std_write(buf);
    }
}

int cat(void *args[]){
    char buf[0x200];
    if(args[1]){
        for(int i=1;args[i];++i){
            if(strcmp(args[i],"-")){
                cat_from_stdin();
            }else{
                int fd=vfs->open(args[i]),nread=0;
                do{
                    nread=vfs->read(fd,buf,sizeof(buf));
                    vfs->write(STDOUT,buf,nread);
                }while(nread>0);
            }
        }
    }else{
        cat_from_stdin();
    }
    return 1;
}
