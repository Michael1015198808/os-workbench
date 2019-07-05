#include <common.h>
#include <amtrace.h>
#include <klib.h>
#include <devices.h>
#include <buildin.h>

#define tty_write(tty, string) \
    tty->ops->write(tty, 0, string, strlen(string)+1)

#define pair(command) \
    {#command, command}
static struct Command{
    const char *name;
    int(*const binary)(void*[],device_t*);
}buildin[]={
    pair(echo),
    pair(cat)
};
void mysh(void *name) {
    device_t *tty = dev_lookup(name);
    while (1) {
        char input[128], prompt[128];
        void *args[10];
        sprintf(prompt, "(%s) $ ", name);
        tty_write(tty,prompt);
        //tty->ops->write(tty, 0, prompt, strlen(prompt)+1);
        int nread = tty->ops->read(tty, 0, input, sizeof(input));
        do{
            args[0]=input;
            int i=1,j=1;
            for(;i<=nread;++i){
                if(input[i-1]==' '){
                    input[i-1]='\0';
                    args[j]=input+i;
                    ++j;
                }
            }
            args[j]=NULL;
        }while(0);
        for(int i=0;args[i];++i){
            printf("args[%d]:%s\n",i,args[i]);
        }
        for(int i=0;;++i){
            if(i==LEN(buildin)){
                char warn[]="mysh: command not found: ";
                tty_write(tty,warn);
                tty_write(tty,args[0]);
                tty_write(tty,"\n");
                break;
            }else
            if(!strcmp(input,buildin[i].name)){
                buildin[i].binary(args,tty);
                break;
            }
        }
    }
}
