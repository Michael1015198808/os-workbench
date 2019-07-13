#include <klib.h>
#include <devices.h>

int mysh_cd(void* args[]){
    char* pwd=args[0];
    char* input=args[1]+2;
    (void)pwd;
    (void)input;
    TODO();
    return -1;
}
