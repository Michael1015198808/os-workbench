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
    char **new_argv=(char**)malloc(sizeof(void*)*argc);
    if(new_argv==NULL){
      printf("No space for new_argv\n");
      fflush(stdout);
      exit(1);
    }
    int i;
    for(i=1;i<=argc;++i){
      new_argv[i-1]=argv[i];
    }
    new_argv[argc]=NULL;
    printf("argv[1]:%s\n",argv[1]);
    execve(new_argv[0],new_argv,envp);
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
