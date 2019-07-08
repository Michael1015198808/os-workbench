#include <dir.h>
#include <klib.h>
#include <devices.h>

static inline void single_ls(const char* path,int* err){
    int fd=vfs->open(path,O_RDONLY),nread=0;
    int cnt=0;
    char buf[200];
    while((nread=vfs->read(fd,buf,sizeof(buf)))>0){
        if(cnt+strlen(buf)>40){
            std_write("\n");
        }
        printf("ls:read %d\n",nread);
        cnt+=fprintf(STDOUT,"%s  ",buf);
    }
    if(nread<0){
        *err=-1;
    }
}

//path here may be relative or absolute
static inline void path_to_absolute(const char* path,int* err){
    if(path[0]=='/'){
        //absolute
        single_ls(path,err);
    }else{
        //Relative
        task_t* cur=get_cur();
        dir_cat(cur->pwd,path);
        single_ls(path,err);
    }
}

int mysh_ls(void *args[]){
    int err=0;
    if(args[1]){
        if(args[2]){
            for(int i=1;args[i];++i){
                std_write(args[i]);
                std_write(":\n");
                single_ls(args[i],&err);
            }
        }else{
            single_ls(args[1],&err);
        }
    }else{
        single_ls(".",&err);
    }
    std_write("\n");
    return err;
}
