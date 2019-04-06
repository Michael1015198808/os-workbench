#include <unistd.h>
#include <stdio.h>
int main(int argc, char *argv[],char *envp[]) {
  char s[100]={};
  int pipes[2];
  /*if(!pipe(pipes)){
      int backup=dup(1);
      dup2(pipes[1],1);
      printf("Hello, world!\n");
      fflush(stdout);
      dup2(backup,1);
      backup=dup(0);
      dup2(pipes[0],0);
      scanf("%s\n",s);
      dup2(backup,0);
      puts(s);
  }*/
  /*if(fork()==0){
    printf("Hello\n");
  }*/
  return 0;
}
