#include <common.h>
#include <amtrace.h>
#include <klib.h>
#include <devices.h>
#include <shell.h>

#define IN_BUILDIN_C
#include <help.h>

#define pair(command) \
    { \
        #command, \
        command,  \
        help_##command \
    }

static struct Command{
    const char *const name;
    int(*const binary)(void*[]);
    const char *const help;
}buildin[]={
    pair(echo),
    pair(cat)
};

int exec_buildin(const char* file,void* args[],int *is_buildin){
    for(int i=0;;++i){
        if(i==LEN(buildin)){
            *is_buildin=0;
            return 0;
        }else
        if(!strcmp(file,buildin[i].name)){
            *is_buildin=1;
            return buildin[i].binary(args);
        }
    }
}
