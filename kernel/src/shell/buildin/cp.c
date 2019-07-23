#include <devices.h>
#include <klib.h>
#include <vfs.h>
#include <yls.h>
#include <dir.h>

int mysh_cp(void *args[]){
    if(args[1]&&args[2]){
        if(args[3]){
            fprintf(2,"To much operand\nUsage: mv SOURCE DEST\n  or:  mv FILE DIRECTORY\n");
            return -1;
        }
        char dest[0x100];
        if(vfs->access(args[2],O_RDONLY|O_DIRECTORY)){
            if(vfs->access(args[2],O_RDONLY|O_CREAT)){
                error_print("cannot move file '%s' to '%s' : ",args[1],args[2]);
            }else{
                strcpy(dest,args[2]);
                vfs->unlink(args[2]);
            }
        }else{
            strcpy(dest,args[2]);
            strcat(dest,"/");
            strcat(dest,args[1]+get_last_slash(args[1])+1);
        }
        clear_warn();
        int fd[2],nbyte=0;
        char buf[0x110];
        fd[0]=vfs->open(args[1],O_RDONLY);
        fd[1]=vfs->open(dest,O_WRONLY|O_CREAT);
        while(( nbyte=vfs->read(fd[0],buf,0x100) )){
            vfs->write(fd[1],buf,nbyte);
        }
        error_print("");
        return 0;
    }else{
        fprintf(2,"Missing operand\nUsage: mv SOURCE DEST\n  or:  mv FILE DIRECTORY\n");
    }
    return -1;
}


