#include <klib.h>
#include <devices.h>

int mysh_echo(void *args[]){
    int i=1;
    while(1){
        std_write(args[i]);
        ++i;
        if(args[i]){
            std_write(" ");
        }else{
            std_write("\n");
            return 0;
        }
    }
}
