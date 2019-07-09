#include <klib.h>
#include <devices.h>

int mysh_mkdir(void *args[]){
    if(args[1]){
        task_t* cur=get_cur();
        for(int i=1;args[i];++i){
            vfs->mkdir(args[i]);
        }
        return 0;
    }else{
        fprintf(2, "Usage: mkdir files...\n");
        return -1;
    }
}

