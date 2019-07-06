#include <klib.h>
#include <devices.h>

int mysh_help(void *args[]){
    for(int i=0;args[i];++i){
        const char* const buildin_help(void*);
        const char* const help=buildin_help(args[i]);
        if(help){
            std_write(help);
        }else{
            std_write("help: no help topics match `");
            std_write(args[i]);
            std_write("'");
        }
    }
    return 0;
}
