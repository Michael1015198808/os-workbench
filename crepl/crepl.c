#include <stdio.h>
#include <dlfcn.h>
#include <assert.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#define CC \
    "/usr/bin/gcc"
#define my_write(_fd,_str) \
    write(_fd,_str,strlen(_str))
char cmd[1<<10],out[16],src[16];
char *cflags[]={
    "gcc",
    //"-###",
    "-fPIC",
    "-MMD",
    "-ggdb3",
    "-fomit-frame-pointer",
    "-c",
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
    //Create temp file
    char file[]="XXXXXX";
    int fd=mkstemp(file);
    printf("%d:%s\n",fd,file);
    my_write(fd,"int fun(){return ");
    my_write(fd,cmd);
    my_write(fd,";}");
    //Compile and link
    strcpy(src,file);
    sprintf(out,"%s.o",src);
    if(!fork()){
        execve(CC,cflags,envp);
    }
    getchar();
    unlink(file);
    void *handle;
    printf("%s\n",out);
    assert(handle=dlopen(out, RTLD_LAZY | RTLD_DEEPBIND));
    int (*fun)(void)= dlsym(handle, "fun");
    assert(fun);
    fun();

    /*if(suffix_of("int",cmd)){
    }else{
        break;
    }*/
  }
  return 0;
}
int suffix_of(char *s1,char *s2){
    while(*s1&&*s1==*s2)
        ++s1,++s2;
    return *s1=='\0';
}
