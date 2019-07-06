#include <common.h>
#include <amtrace.h>
#include <klib.h>
#include <devices.h>
#include <buildin.h>

#define tty_write(tty, string) \
    tty->ops->write(tty, 0, string, strlen(string))

#define pair(command) \
    {#command, command}
static struct Command{
    const char *name;
    int(*const binary)(void*[]);
}buildin[]={
    pair(echo),
    pair(cat)
};
static void inline command_handler(char *input,void *args[10],int nread){
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
        int temp=vfs->open(name,7);
        Assert(temp==0,"Error on fd-stdin:%d",temp);
        temp=vfs->open(name,7);
        Assert(temp==1,"Error on fd-stdout:%d",temp);
    }
    device_t* tty=dev_lookup(name);//For debugging
    while (1) {
        char input[128], prompt[128];
        void *args[10];
        sprintf(prompt, "(%s) $ ", name);
        tty_write(tty,prompt);
        //vfs->write(1,info(prompt));
        int nread = tty->ops->read(tty,0,info(input));
        //int nread = vfs->read(0,info(input));
        input[nread-1]='\0';

        command_handler(input,args,nread);
        for(int i=0;args[i];++i){
            printf("args[%d]:%s\n",i,args[i]);
        }
        for(int i=0;;++i){
            if(i==LEN(buildin)){
                char warn[]="mysh: command not found: ";
                tty_write(tty,warn);
                tty_write(tty,args[0]);
                tty_write(tty,"\n");
                //vfs->write(1,info(warn));
                //vfs->write(1,info(args[0]));
                //vfs->write(1,info("\n"));
                break;
            }else
            if(!strcmp(input,buildin[i].name)){
                buildin[i].binary(args);
                break;
            }
        }
    }
}
