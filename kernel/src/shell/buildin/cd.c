#include <klib.h>
#include <devices.h>

int mysh_cd(void* args[]){
    char* pwd=args[0];
    char* input=args[1]+2;
    (void)pwd;
    (void)input;
    fprintf(2,"Sorry, cd haven't been realized!");
    return -1;
}
