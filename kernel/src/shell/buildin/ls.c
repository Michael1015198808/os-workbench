#include <dir.h>
#include <klib.h>
#include <devices.h>
#include <vfs.h>

static inline void single_ls(const char* path,int* err){
    int fd=vfs->open(path,O_RDONLY | O_DIRECTORY),nread=0;
    if(fd<0){error_print("%s: ",path);return;}
    int cnt=0;
    char buf[200];

    while((nread=vfs->readdir(fd,buf,sizeof(buf)))>0){
        if(cnt+strlen(buf)>80){
            std_write("\n");
            cnt=0;
        }
        cnt+=fprintf(STDOUT,"%s  ",buf);
    }
    if(nread<0){
        *err=-1;
    }
    vfs->close(fd);
    if(cnt)
        std_write("\n");
}

int mysh_ls(void *args[]){
    int err=0,i=1;
    if(args[1]){
        if(args[2]){
            for(i=1;args[i];++i){
                fprintf(STDOUT,"%s:\n",args[i]);
                single_ls(args[i],&err);
                if(args[i+1]){std_write("\n");}
            }
        }else{
            single_ls(args[1],&err);
        }
    }else{
        single_ls(".",&err);
    }
    return err;
}

/* ls
 * ls .
 * ls /
 * ls test
 * ls test
 * ls /test
 * ls test/
 * ls /test/
 * ls / test/
 */
