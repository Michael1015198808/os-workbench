#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>
#include <stdio.h>
void stop(void){
  fflush(stdout);
  exit(1);
}
regex_t name,num;
regmatch_t match_info;
int main(int argc, char *argv[],char *envp[]) {
  if(argc==1){
    fprintf(stderr,"sperf: must have PROG [ARGS]\n");
    stop();
  }
  int pipes[2];
  if(pipe(pipes)){
    printf("Build pipe failed!\n");
    stop();
  }
  //compile regexs
  if(
    regcomp(&name,"^[a-zA-Z]*\\(",REG_EXTENDED) ||
    regcomp(&num,"<[0-9\\.]*>$",REG_EXTENDED) ){
      printf("Regexes compiled failed!\n");
      stop();
  }
  int ret=fork();
  if(ret==-1){
    printf("Fork failed!\n");
    stop();
  }
  if(ret==0){
    //Child process
    //Prepare new_argv[]
    const int new_argc=argc+1;
    char **new_argv=(char**)malloc(sizeof(void*)*(new_argc+1));
    if(new_argv==NULL){
      printf("No space for new_argv\n");
      fflush(stdout);
      exit(1);
    }
    new_argv[0]="/usr/bin/strace";
    new_argv[1]="-T";
    memcpy(new_argv+2,argv+1,argc*sizeof(void*));
    new_argv[new_argc]=NULL;
    //Prepare file descriptors
    int backup[2];
    backup[0]=dup(1);
    backup[1]=dup(2);
    close(1);
    dup2(pipes[1],2);
    execve("/usr/bin/strace",new_argv,envp);
    dup2(backup[0],1);
    dup2(backup[1],2);
    printf("%s:%d Should not reach here!\n",__FILE__,__LINE__);
    fflush(stdout);
    stop();
  }else{
    //Parent process
    dup2(pipes[0],0);
    char s[256];
    char call[20];
    //double time;
    while(fgets(s,sizeof(s),stdin)>=0){
      if(regexec(&name,s,1,&match_info,0)==REG_NOMATCH){
        continue;
      }
      strncpy(call,s+match_info.rm_so,match_info.rm_eo-match_info.rm_so);
      call[match_info.rm_eo-match_info.rm_so+1]='\0';
      printf("%s\n",call);
    }
  }
  return 0;
}
