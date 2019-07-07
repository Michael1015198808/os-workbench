#include <klib.h>
#include <devices.h>
const char *warn="Haven't realized!\n";

static inline void cat_from_stdin(char buf[0x200]){
    int nread=0;
    while((nread=vfs->read(STDIN,buf,0x200-1))>0){
        printf("cat read %d bytes\n",nread);
        buf[nread]='\0';
        std_write(buf);
    }
    return (nread==0)?0:-1;
}

int mysh_cat(void *args[]){
    char buf[0x200];
    if(args[1]){
        for(int i=1;args[i];++i){
            if(strcmp(args[i],"-")){
                cat_from_stdin(buf);
            }else{
                int fd=vfs->open(args[i],7),nread=0;
                do{
                    nread=vfs->read(fd,buf,sizeof(buf));
                    vfs->write(STDOUT,buf,nread);
                }while(nread>0);
            }
        }
    }else{
        cat_from_stdin(buf);
    }
    return 1;
}
