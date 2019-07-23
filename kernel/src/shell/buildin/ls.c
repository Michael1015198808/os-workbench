#include <dir.h>
#include <klib.h>
#include <devices.h>
#include <vfs.h>

static inline void single_ls(const char* path,int* err){
    int fd=vfs->open(path,O_RDONLY | O_DIRECTORY),nread=0;
    if(fd<0){error_print("%s: ",path);return;}
    int cnt=0,len=strlen(path);
    int tty_mode=isatty(STDOUT);
    char buf[200],file[200];
    uint32_t ori_colors[2],dir_colors[2];
    device_t* tty=get_cur()->fd[STDOUT]->inode->ptr;
    if(tty_mode){
        tty_get_color(tty,ori_colors);
        dir_colors[1]=ori_colors[1];
        dir_colors[0]=0x00006fb8;
        strcpy(file,path);
        if(path[len-1]!='/'){
            file[len++]='/';
        }
    }

    volatile int stop_val=0;
    while((nread=vfs->readdir(fd,buf,sizeof(buf)))>0){
        while(stop_val);
        if(tty_mode){
            strcpy(file+len,buf);
            if(vfs->access(file,O_DIRECTORY)){
                tty_set_color(tty,ori_colors);
            }else{
                tty_set_color(tty,dir_colors);
            }
            error_clear();
            if(cnt+strlen(buf)>80){
                std_write("\n");
                cnt=0;
            }
            cnt+=fprintf(STDOUT,"%s  ",buf);
        }else{
            fprintf(STDOUT,"%s\n",buf);
        }
    }
    if(nread<0){
        *err=-1;
    }
    if(tty_mode){
        tty_set_color(tty,ori_colors);
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
