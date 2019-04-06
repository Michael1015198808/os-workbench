#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>
#include <stdio.h>
#include <time.h>
void stop(void){
  fflush(stdout);
  exit(1);
}
typedef struct node{
    char *name;
    double time;
    struct node* next;
}node;
node head={"\1\0",0,&head};
double total=0.0;
regex_t name,num;
regmatch_t match_info;
void sort(void);
void display(void);
inline void swap(node*,node*,node*);
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
    regcomp(&num,"<[0-9\\.]*>\n",REG_EXTENDED) ){
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
    double time_cost;
    time_t oldtime=0,newtime;
    while(fgets(s,sizeof(s),stdin)>=0){
      //Get name of syscall
      if(regexec(&name,s,1,&match_info,0)==REG_NOMATCH){
        continue;
      }
      strncpy(call,s+match_info.rm_so,match_info.rm_eo-match_info.rm_so);
      call[match_info.rm_eo-match_info.rm_so-1]='\0';
      //Get time of syscall
      if(regexec(&num,s,1,&match_info,0)==REG_NOMATCH){
        continue;
      }
      sscanf(s+match_info.rm_so+1,"%lf",&time_cost);
      //Record 
      node *p=&head,*q=NULL;
      do{
          q=p;
          p=p->next;
      }while(p!=&head&&strcmp(p->name,call)!=0);
      if(strcmp(p->name,call)){
        //before q----->p
        //after  q-new->p
        q->next=(node*)malloc(sizeof(node));
        q->next->next=p;
        q=q->next;
        q->name=(char*)malloc(strlen(call)+1);
        strcpy(q->name,call);
        q->time=0.0;
      }
      q->time+=time_cost;
      total+=time_cost;
      if(time(&newtime)>oldtime){
        oldtime=newtime;
        sort();
        display();
      }
    }
  }
  return 0;
}
void sort(void){
  //bubble sort
  printf("sort\n");
  node *p=head.next;
  for(p=&head;p!=&head;){
      //r->q--..->p
      node *q=head.next,*r=&head;
      while(q!=p){
          if(
            (q->time)<
            (q->next->time)
          ){
              swap(r,q,q->next);
          }
          r=q;
          q=q->next;
      }
      p=r;
  }
}
void display(void){
  node *p=head.next;
  printf("\033[2J\033[39m\033[49mSyscalls:\n");
  do{
    printf("%10s:%10lf(%.2lf%%)\n",p->name,p->time,p->time*100/total);
    p=p->next;
  }while(p!=&head);
}
inline void swap(node *p,node *a,node *b){
//Corectness first
    node *list[]={p,b,a,b->next};
    int i;
    for(i=0;i<3;++i){
        list[i]->next=list[i+1]->next;
    }
    return;
}
