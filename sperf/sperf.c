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
    const int new_argc=argc+1;
    char **new_argv=(char**)malloc(sizeof(void*)*(new_argc+1));
    if(new_argv==NULL){
      printf("No space for new_argv\n");
      fflush(stdout);
      exit(1);
    }
    int i;
    new_argv[0]="/usr/bin/strace";
    new_argv[1]="-T";
    for(i=2;i<new_argc;++i){
      new_argv[i]=argv[i-1];
    }
    new_argv[new_argc]=NULL;
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
      dup2(pipes[0],0);
      int c;
      while((c=getchar())!=EOF){
          putchar(c);
      }
    }
  return 0;
}
