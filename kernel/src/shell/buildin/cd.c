#include <klib.h>
#include <devices.h>

inode_t* vfs_lookup(const char* path,int flags);

int mysh_cd(void* args[]){
    char* input=args[1];
    if(input[0]=='/'){
        inode_t* next=vfs_lookup(input,O_RDONLY|O_DIRECTORY);
        if(next){
            task_t* cur=get_cur();
            cur->cur_dir=next;
            strcpy(cur->pwd,input);
            return 0;
        }
    }else{
        TODO();
    }
    return -1;
}
