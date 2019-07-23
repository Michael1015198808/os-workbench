#include <common.h>
#include <amtrace.h>
#include <klib.h>
#include <devices.h>
#include <shell.h>


#define pair(command,help) \
    { \
        #command, \
        mysh_##command,  \
        help \
    }

static struct Command{
    const char *const name;
    int(*const binary)(void*[]);
    const char *const help;
}buildin[]={
    pair(echo   ,"Write arguments to the standard output."),
    pair(cat    ,"Concatenate FILE(s) to standard output."),
    pair(help   ,"Display information about builtin commands or commands with help information in help.h."),
    pair(cd     ,"Change the shell working directory."),
    pair(wc     ,"Print newline, word, and byte counts for each FILE, and a total line if more than one FILE is specified."),
    pair(ls     ,"List information about the FILEs (the current directory by default)."),
    pair(mkdir  ,"Create the DIRECTORY(ies), if they do not already exist."),
    pair(sleep  ,"Pause for NUMBER seconds."),
    pair(touch  ,"Update the access and modification times of each FILE to the current time."),
    pair(test   ,"A test command. Now is used to test pipe"),
    pair(link   ,"Call the link function to create a link named FILE2 to an existing FILE1."),
    pair(unlink ,"Call the unlink function to remove the specified FILE."),
    pair(rm     ,"Remove (unlink) the FILE(s)."),
    pair(mv     ,"Rename SOURCE to DEST, or move SOURCE to DIRECTORY."),
    pair(cp     ,"Copy SOURCE to DEST, or copy SOURCE to DIRECTORY."),
    pair(color  ,"Set color of current terminal."),
    pair(rmdir  ,"Remove the DIRECTORY(ies), if they are empty."),
};

int exec_buildin(const char* file,void* args[],int *is_buildin){
    for(int i=0;i!=LEN(buildin);++i){
        if(!strcmp(file,buildin[i].name)){
            *is_buildin=1;
            task_t* cur=get_cur();
            pmm->free(cur->name);
            cur->name=pmm->alloc(strlen(buildin[i].name)+1);
            strcpy(cur->name,buildin[i].name);
            return buildin[i].binary(args);
        }
    }
    *is_buildin=0;
    return 0;
}
const char* const buildin_help(void* arg){
    for(int i=0;;++i){
        if(i==LEN(buildin)){
            return NULL;
        }else
        if(!strcmp(arg,buildin[i].name)){
            return buildin[i].help;
        }
    }
}
