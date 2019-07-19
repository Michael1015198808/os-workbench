#include <klib.h>
#include <devices.h>
#include <vfs.h>

inode_t* vfs_lookup(const char* path,int flags);

int mysh_cd(void* args[]){
    char* input=args[1];
    if(vfs->chdir(input)){
        //Error handle
        TODO();
        return -1;
    }
    return 0;
}
