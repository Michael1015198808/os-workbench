#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
void stop(void){
  fflush(stdout);
  exit(1);
}
int main(int argc, char *argv[],char *envp[]) {
  //char s[100]={};
  int pipes[2];
  if(pipe(pipes)){
    printf("Build pipe failed!\n");
    stop();
  }
  int ret;
  if(!(ret=fork())){
    //Child process
    char **new_argv=(char**)malloc(sizeof(void*)*(argc+1));
    if(new_argv==NULL){
      printf("No space for new_argv\n");
      fflush(stdout);
      exit(1);
    }
    int i;
    for(i=2;i<=argc+1;++i){
      new_argv[i-2]=argv[i];
    }
    new_argv[argc]=NULL;
    for(i=0;i<argc+1;++i){
        printf("new_argv[%d]:%s\n",i,new_argv[i]);
    }
    execve("/usr/bin/strace",new_argv,envp);
    printf("%s:%d Should not reach here!\n",__FILE__,__LINE__);
    stop();
  }else
    //Parent process
    if(ret==-1){
      printf("Fork failed!\n");
      stop();
    }else{
      //Fork success
    }
  return 0;
}
