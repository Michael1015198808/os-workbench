/*
 * Author: Michael Yan
 * STUID: 171240518
 * Features:
 *      Use readline to provide more flexibility
 *      Auto clear output files(*.so,*.c etc)
 *      Input exit to stop
 *      Assertions
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
#define CC \
    "/usr/bin/gcc"
#define log(format,...) \
    fprintf(stderr,"Line %3d: " format,__LINE__, __VA_ARGS__)
#define my_write(_fd,_str) \
    write(_fd,_str,strlen(_str))
char *cmd=NULL,out[16],src[16];
char *cflags[]={
    "gcc",
    "-fPIC",
#if defined(__i386__)
    "-m32",
#elif defined(__x86_64__)
    "-m64",
#endif
    "-Werror",
    "-shared",
    "-x",
    "c",
    "-o",
    out,
    src,
    NULL
};
int suffix_of(char *,char *);
void *handle;
sigjmp_buf interpreter;
void restore(int errno){
    longjmp(interpreter);
}
int main(int argc, char *argv[],char *envp[]) {
  signal(SIGSEG,restore);
  setjmp(interpreter);
  while(1){
    //Input
    if(cmd){free(cmd);cmd=NULL;}
    cmd=readline(">> ");
    if(cmd&&*cmd){
        add_history(cmd);
    }
    if(!strcmp("exit",cmd))return 0;

    //Create temp file
    char file[]="XXXXXX";
    int fd=mkstemp(file);
    int add_func=suffix_of("int",cmd);
    if(fd==0)log("%s","Can't create temporary file!\n");
    log("%d:%s\n",fd,file);
    if(add_func){
        my_write(fd,cmd);
    }else{
        my_write(fd,"int __expr_wrapper(){return ");
        my_write(fd,cmd);
        my_write(fd,";}");
    }

    //Compile and link
    strcpy(src,file);
    sprintf(out,"./%s.so",src);
    if(!fork()){
        execve(CC,cflags,envp);
    }
    uint32_t wstatus;
    wait((void*)&wstatus);
    unlink(file);
    if(wstatus!=0){
        printf("%d" ":%s",wstatus, "Compile error!\n");
        continue;
    }
    handle=dlopen(out, RTLD_LAZY|RTLD_GLOBAL);
    if(!handle){
        printf("load failed!\n");
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
    unlink(out);
  }
  return 0;
}
int suffix_of(char *s1,char *s2){
    while(*s1&&*s1==*s2)
        ++s1,++s2;
    return *s1=='\0';
}
