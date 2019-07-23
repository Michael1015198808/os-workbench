#include <devices.h>
#include <vfs.h>

extern uint32_t atoi16(const char* nptr);

static inline int set_color_real(void *args[],device_t* tty,uint32_t* colors){
    int len[2]={
        strlen(args[1]),strlen(args[2])
    };

    if(!args[1]){
            colors[0]=DEFAULT_FG;
            colors[1]=DEFAULT_BG;
            return 0;
    }
    if(args[1]&&args[2]&&!args[3]){
        if(len[0]==2){
            if(!strcmp(args[1],"bg")||!strcmp(args[1],"BG")){
                colors[1]=atoi16(args[2]);
                return 0;
            }else if(!strcmp(args[1],"fg")||!strcmp(args[1],"FG")){
                colors[0]=atoi16(args[2]);
                return 0;
            }
        }else if(len[0]==6&&len[1]==6){
            colors[0]=atoi16(args[1]);
            colors[1]=atoi16(args[2]);
            return 0;
        }
    }
    fprintf(2,"Usage: color\n  or:  color [fg|bg] COLOR\n  or:  color COLOR COLOR\nFORMAT: ffffff\n");
    return -1;
}

int mysh_color(void *args[]){
    Assert(isatty(STDOUT),"Standard output isn't a terminal!, can't set color");
    uint32_t colors[2];
    device_t* tty=get_cur()->fd[STDOUT]->inode->ptr;

    tty_get_color(tty,colors);
    int ret=set_color_real(args,tty,colors);
    tty_set_color(tty,colors);

    return ret;
}

