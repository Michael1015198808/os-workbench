#include <common.h>
#include <amtrace.h>
#include <klib.h>
#include <devices.h>
#include <buildin.h>

static struct Command{
    const char *name;
    int(*const binary)(void*[],device_t*);
}buildin[]={
    {"echo",echo}
};
void mysh(void *name) {
    device_t *tty = dev_lookup(name);
    while (1) {
        char input[128], prompt[128];
        void *args[10];
        sprintf(prompt, "(%s) $ ", name);
        tty->ops->write(tty, 0, prompt, strlen(prompt)+1);
        int nread = tty->ops->read(tty, 0, input, sizeof(input));
        input[nread-1]=' ';
        for(int i=1,j=0;i<=nread;++i){
            if(input[i-1]==' '){
                input[i-1]='\0';
                args[j]=input+i;
                ++j;
            }
        }
        for(int i=0;i<LEN(buildin);++i){
            if(!strcmp(input,buildin[i].name)){
                buildin[i].binary(args,tty);
                break;
            }
        }
    }
}
