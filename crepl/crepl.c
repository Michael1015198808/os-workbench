#include <stdio.h>
#include <dlfcn.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
char cmd[1000];
char *cflags[]={
    "-g",
    "-c",
};
int suffix_of(char *,char *);
int main(int argc, char *argv[]) {
  while(1){
    printf(">> ");
    fgets(cmd,sizeof(cmd),stdin);
    char file[]="XXXXXX";
    int fd=mkstemp(file);
    printf("%d:%s\n",fd,file);
    write(fd,cmd,strlen(cmd));
    getchar();
    unlink(file);
    void *handle;
    handle=dlopen(file, RTLD_LAZY | RTLD_DEEPBIND);
    void (*fun)(void)= dlsym(handle, "fun");
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
