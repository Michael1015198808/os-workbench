#include <stdio.h>
#include <stdlib.h>
char cmd[1000];
int suffix_of(char *,char *);
int main(int argc, char *argv[]) {
  while(1){
    printf(">> ");
    fgets(cmd,sizeof(cmd),stdin);
    if(suffix_of("int",cmd)){
    }else{
        char file[]="XXXXXX";
        int fd=mkstemp(file);
        printf("%s\n",file);
        getchar();
        break;
    }
  }
  return 0;
}
int suffix_of(char *s1,char *s2){
    while(*s1&&*s1==*s2)
        ++s1,++s2;
    return *s1=='\0';
}
