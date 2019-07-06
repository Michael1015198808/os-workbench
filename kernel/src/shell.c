#include <common.h>
#include <amtrace.h>
#include <klib.h>
#include <devices.h>
#include <buildin.h>

#define pair(command) \
    { \
        #command, \
        command,  \
        MACRO_CONCAT(help_,MACRO_SELF(command)) \
    }

static struct Command{
    const char *const name;
    int(*const binary)(void*[]);
    const char *const help;
}buildin[]={
    pair(echo),
    pair(cat)
};
static void inline command_handler(char *input,void *args[10],int nread){
    input[nread-1]='\0';
    do{
        args[0]=input;
        int i=1,j=1;
        for(;i<=nread;++i){
            if(input[i-1]==' '){
                input[i-1]='\0';
                if(input[i]!=' '){
                    args[j]=input+i;
                    ++j;
                }
            }
        }
        args[j]=NULL;
    }while(0);
}
void mysh(void *name) {
    {
        int temp=-1;
        temp=vfs->open(name,7);
        Assert(temp==0,"fd of stdin should be 0, instead of %d!\n",temp);
        temp=vfs->open(name,7);
        Assert(temp==1,"fd of stdin should be 1, instead of %d!\n",temp);
    }
    while (1) {
        char input[128], prompt[128];
        void *args[10];
        sprintf(prompt, "(%s) $ ", name);
        std_write(prompt);
        int nread=std_read(input);
        
        command_handler(input,args,nread);
        for(int i=0;args[i];++i){
            printf("args[%d]:%s\n",i,args[i]);
        }
        for(int i=0;;++i){
            if(i==LEN(buildin)){
                char warn[]="mysh: command not found: ";
                std_write(warn);
                std_write(args[0]);
                std_write("\n");
                break;
            }else
            if(!strcmp(input,buildin[i].name)){
                buildin[i].binary(args);
                break;
            }
        }
    }
}
