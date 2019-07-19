#include <klib.h>
#include <devices.h>
#include <vfs.h>

inode_t* vfs_lookup(const char* path,int flags);

static inline const char* get_home(void){
    //temp
    return "/";
}

int mysh_cd(void* args[]){
    const char* dir=args[1];
    if(!dir)dir=get_home();

    if(vfs->chdir(dir)){
        //Error handle
        error_print("cd: %s: ",args[1]);
        return -1;
    }
    return 0;
}
