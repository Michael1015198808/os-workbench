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

#define err(...) \
    fprintf(stderr, __VA_ARGS__)

#define log(format,...) \
    err("Line %3d: " format,__LINE__, __VA_ARGS__)

char *cmd=NULL,out[16],src[8];
char *cflags[]={
    "gcc",
    "-fPIC",
#if defined(__i386__)
    "-m32",
#elif defined(__x86_64__)
    "-m64",
#endif
    //"-Werror",
    "-shared",
    "-x",
    "c",
    "-o",
    out,
    src,
    NULL
};
void *handle;
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
        if(fd>0){
            int add_func=!strncmp("int",cmd,3);
            if(add_func){
                dprintf(fd,"%s",cmd);
            }else{
                dprintf(fd,"int __expr_wrapper(){return %s;}",cmd);
            }
        
            //Compile and link
            strcpy(src,file);
            sprintf(out,"./%s.so",src);
            if(!fork()){
                close(1);close(2);
                execve(CC,cflags,envp);
            }
            uint32_t wstatus;
            wait((void*)&wstatus);
            unlink(file);
            if(wstatus!=0){
                err("%d" ":%s",wstatus, "Compile error!\n");
                continue;
            }
            handle=dlopen(out, RTLD_LAZY|RTLD_GLOBAL);
            unlink(out);
            if(!handle){
                err("load failed!\n");
                continue;
            }
            if(add_func){
                //Add a function
                printf("Added: %s\n",cmd);
            }else{
                //Calculate the value
                int (*fun)(void)= dlsym(handle, "__expr_wrapper");
                assert(fun);
                printf("(%s) == %d\n",cmd,fun());
            }
        }else{
            log("%s","Can't create temporary file!\n");
        }
    }
    return 0;
}
