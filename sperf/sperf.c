#include <unistd.h>
#include <stdio.h>
int main(int argc, char *argv[]) {
  char s[100]={};
  int pipes[2];
  if(!pipe(pipes)){
      write(pipes[0],"Hello, world!\n",7);
      read(pipes[1],s,10);
      puts(s);
  }
  /*if(fork()==0){
    printf("Hello\n");
  }*/
  return 0;
}
