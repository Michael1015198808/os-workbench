#include <stdio.h>
#include <stdint.h>
#include <inttypes.h>
#include <dlfcn.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#define CC \
    "/usr/bin/gcc"
#define log(format,...) \
    fprintf(stderr,"Line %3d: " format,__LINE__, __VA_ARGS__)
#define my_write(_fd,_str) \
    write(_fd,_str,strlen(_str))
char cmd[1<<10],out[16],src[16];
char *cflags[]={
    "gcc",
    //"-###",
    "-fPIC",
#if defined(__i386__)
    "-m32",
#elif defined(__x86_64__)
    "-m64",
#endif
    //"-MMD",
    //"-ggdb3",
    //"-fomit-frame-pointer",
    "-shared",
    //"-c",
    "-x",
    "c",
    "-o",
    out,
    src,
    NULL
};
int suffix_of(char *,char *);
int main(int argc, char *argv[],char *envp[]) {
  while(1){
    //Input
    printf(">> ");
    fgets(cmd,sizeof(cmd),stdin);
    cmd[strlen(cmd)-1]='\0';//Remove the \n
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
        my_write(fd,"int fun(){return ");
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
        printf("%ld" ":%s",wstatus, "Compile error!\n");
        continue;
    }
    void *handle;
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
        int (*fun)(void)= dlsym(handle, "fun");
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
