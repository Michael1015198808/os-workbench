#include <klib.h>
#include <devices.h>
#include <vfs.h>

int mysh_help(void *args[]){
    if(!args[1]){
        std_write("Type `help name' to find out more about the function `name'.\n");
        return 0;
    }
    for(int i=1;args[i];++i){
        const char* const buildin_help(void*);
        void* help=(void*)buildin_help(args[i]);
        if(help){
            std_write(help);
        }else{
            std_write("help: no help topics match `");
            std_write(args[i]);
            std_write("'");
        }
        std_write("\n");
    }
    return 0;
}
