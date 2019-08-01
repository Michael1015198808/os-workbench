/*
 * Author: Michael Yan
 * STUID: 171240518
 * Features:
 *      Use readline to provide more flexibility
 *      Auto clear output files(*.so,*.c etc)
 *      Input exit to stop
 */
#include <stdio.h>
#include <setjmp.h>
#include <signal.h>
#include <stdint.h>
#include <inttypes.h>
#include <dlfcn.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <readline/readline.h>
#include <readline/history.h>
int dprintf(int fd,const char* format,...);

#define CC \
    "/usr/bin/gcc"

#define Assert(cond, _fmt, ...) \
    do{ \
        if(!(cond)){ \
            fprintf(stderr, _fmt "\n" ## __VA_ARGS__); \
            exit(1); \
        } \
    }while(0)

#define err(...) \
    fprintf(stderr, __VA_ARGS__)

#define log(format,...) \
    err("Line %3d: " format,__LINE__, __VA_ARGS__)

#if defined(__i386__)
    #define MACHINE_OPTION "-m32"
#elif defined(__x86_64__)
    #define MACHINE_OPTION "-m64"
#else
    _Static_assert(0,"Unknown architecture");
#endif
int safe_fork(void){
    int pid=fork();
    Assert(pid>=0,"Fork failed!");
    return pid;
}

char *cmd=NULL,out[12],src[7];
char *cflags[]={
    "gcc",
    "-fPIC",
    MACHINE_OPTION,
    //"-Werror",
    "-Wno-implicit-function-declaration",
    "-shared",
    "-x",
    "c",
    "-o",
    out,
    src,
    NULL
};
inline void main_loop(char* cmd,char** envp);
int main(int argc, char *argv[],char **envp) {
    while(1){
        //Input
        free(cmd);
        cmd=readline(">> ");
        if(cmd&&*cmd){
            add_history(cmd);
        }
        if(strcmp("exit",cmd)){
            main_loop(cmd,envp);
        }else{
            return 0;
        }
    }
}

char buffer[0x1000];
inline void main_loop(char* cmd,char** envp){
    //Create temp file
    char file[]="XXXXXX";
    int fd=mkstemp(file);
    if(fd<0){
        log("%s","Can't create temporary file!\n");
    }else{
        buffer[0]='\0';
        int add_func=!strncmp("int ",cmd,3);
        if(add_func){
            int remain=0;
            do{
                for(char* c=(char*)cmd;*c;++c){
                    switch(*c){
                        case '{':
                            ++remain;
                            break;
                        case '}':
                            --remain;
                            if(remain<0){err("Too much }");return;}
                            break;
                    }
                }
                strcat(buffer,cmd);
                if(remain==0){
                    break;
                }
                cmd=readline("");
                add_history(cmd);
            }while(1);
            dprintf(fd,"%s",buffer);
        }else{
            dprintf(fd,"int __expr_wrapper(void){return %s;}",cmd);
        }
        close(fd);

         //Compile and link
        sprintf(out,"./%s.so",file);
        if(safe_fork()==0){
            strcpy(src,file);
            execve(CC,cflags,envp);
            err("call " CC " failed!\n");
            exit(1);
        }else{
            int wstatus;
            wait(&wstatus);
            unlink(file);
            if(wstatus!=0){
                err("%d" ":%s",wstatus, "Compile error!\n");
                return;
            }

            if(add_func){
                void *handle=dlopen(out, RTLD_LAZY|RTLD_GLOBAL);
                unlink(out);
                if(!handle){
                    err("load failed!\n");
                    return;
                }
                //Add a function
                printf("Added: %s\n",buffer);
            }else{
                //Calculate the value
                if(safe_fork()==0){
                    void *handle=dlopen(out, RTLD_LAZY|RTLD_GLOBAL);
                    unlink(out);
                    if(!handle){
                        err("load failed!\n");
                        return;
                    }
                    int (*fun)(void)= dlsym(handle, "__expr_wrapper");
                    assert(fun);
                    printf("(%s) == %d\n",cmd,fun());
                    exit(0);
                }else{
                    wait(&wstatus);
                    if(wstatus!=0){
                        err("Something wrong with child process\n");
                    }
                }
            }
        }
    }
}

