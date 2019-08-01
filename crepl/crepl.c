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

char *cmd=NULL,out[12],src[7];
char *cflags[]={
    "gcc",
    "-fPIC",
    MACHINE_OPTION,
    //"-Werror",
    "-shared",
    "-x",
    "c",
    "-o",
    out,
    src,
    NULL
};
int main(int argc, char *argv[],char *envp[]) {
    while(1){
        //Input
        free(cmd);
        cmd=readline(">> ");
        if(cmd&&*cmd){
            add_history(cmd);
        }
        if(!strcmp("exit",cmd))return 0;

        //Create temp file
        char file[]="XXXXXX";
        int fd=mkstemp(file);
        if(fd<0){
            log("%s","Can't create temporary file!\n");
        }else{
            int add_func=!strncmp("int",cmd,3);
            if(add_func){
                dprintf(fd,"%s",cmd);
            }else{
                dprintf(fd,"int __expr_wrapper(){return %s;}",cmd);
            }

            //Compile and link
            sprintf(out,"./%s.so",file);
            int pid=fork();
            Assert(pid>=0,"Fork failed!");
            if(pid==0){
                strcpy(src,file);
                execve(CC,cflags,envp);
                err("call " CC " failed!\n");
                return -1;
            }else{
                uint32_t wstatus;
                wait((void*)&wstatus);
                unlink(file);
                if(wstatus!=0){
                    err("%d" ":%s",wstatus, "Compile error!\n");
                    continue;
                }
                if(add_func){
                    void *handle=dlopen(out, RTLD_LAZY|RTLD_GLOBAL);
                    unlink(out);
                    if(!handle){
                        err("load failed!\n");
                        continue;
                    }
                    //Add a function
                    printf("Added: %s\n",cmd);
                }else{
                    //Calculate the value
                    int pid=fork();
                    Assert(pid>=0,"Fork failed!");
                    if(pid==0){
                        void *handle=dlopen(out, RTLD_LAZY|RTLD_GLOBAL);
                        unlink(out);
                        if(!handle){
                            err("load failed!\n");
                            continue;
                        }
                        int (*fun)(void)= dlsym(handle, "__expr_wrapper");
                        assert(fun);
                        printf("(%s) == %d\n",cmd,fun());
                        return 0;
                    }else{
                        wait((void*)&wstatus);
                    }
                }
            }
        }
    }
    return 0;
}
