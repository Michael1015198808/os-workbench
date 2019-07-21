#include <devices.h>
#include <klib.h>
#include <vfs.h>
#include <yls.h>
#include <dir.h>

int mysh_mv(void *args[]){
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
            }
        }else{
            strcpy(dest,args[2]);
            strcat(dest,"/");
            strcat(dest,args[1]+get_last_slash(args[1])+1);
        }
        vfs->link(args[1],dest);
        vfs->unlink(args[1]);
        error_print("");
        return 0;
    }else{
        fprintf(2,"Missing operand\nUsage: mv SOURCE DEST\n  or:  mv FILE DIRECTORY\n");
    }
    return -1;
}


