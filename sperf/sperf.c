#include <sys/types.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
#define err(...) \
    fprintf(stderr,__VA_ARGS__); \
    stop();
#define my_write(_fd,_str) \
    write((_fd),(_str),strlen(_str))
#define LEN(_array) \
    (sizeof(_array)/sizeof(_array[0]))
void stop(void){
  fflush(stdout);
  exit(1);
}
typedef struct node{
    char *name;
    struct node *next;
    double time;
}node;
node head={
  .name="\1\0",
  .next=&head,
  .time=0
};

double total=0.0;
regex_t name,num,exit_pat;
regmatch_t match_info;
void sort(void);
void display(void);
inline void swap(node*,node*,node*);
int main(int argc, char *argv[],char *envp[]) {
  if(argc==1){
    err("sperf: must have PROG [ARGS]\n");
  }
  int pipes[2];
  if(pipe(pipes)){
    err("Build pipe failed!\n");
  }
  //compile regexs
  if(
    regcomp(&name,"^[a-zA-Z]*[0-9]*\\(",REG_EXTENDED) ||
    regcomp(&num,"<[0-9\\.]*>\n",REG_EXTENDED)  ||
    regcomp(&exit_pat,"exited with [0-9]* ",REG_EXTENDED) ){
      err("Regexes compiled failed!\n");
  }
  int ret=fork();
  if(ret==-1){
    err("Fork failed!\n");
  }
  if(ret==0){
    //Child process
    //Prepare new_argv[]
    char* flags[]={"-T"};
    const int new_argc=argc+LEN(flags);
    char **new_argv=(char**)malloc(sizeof(void*)*(new_argc+1));
    if(new_argv==NULL){
      err("No space for new_argv\n");
    }
    new_argv[0]="/usr/bin/strace";
    int i;
    for(i=0;i<LEN(flags);++i){
        new_argv[i+1]=flags[i];
    }
    memcpy(new_argv+LEN(flags)+1,argv+1,argc*sizeof(void*));
    new_argv[new_argc]=NULL;
    //Prepare file descriptors
    int backup[2];
    backup[0]=dup(1);
    backup[1]=dup(2);
    int temp=open("./stdout_log",(O_RDWR||O_CREAT)&&(~O_APPEND));
    if(temp>0){
      dup2(temp,1);
    }else{
      close(1);
    }
    dup2(pipes[1],2);
    execve("/usr/bin/strace",new_argv,envp);
    dup2(backup[0],1);
    dup2(backup[1],2);
    err("%s:%d Should not reach here!\n",__FILE__,__LINE__);
  }else{
    //Parent process
    dup2(pipes[0],0);
    char s[512];
    char call[20];
    double time_cost;
    time_t oldtime=0,newtime;
    while(fgets(s,sizeof(s),stdin)>0){
      //my_write(3,s);
      if(regexec(&exit_pat,s,1,&match_info,0)!=REG_NOMATCH){
        //returned
        display();
        printf("%s ",argv[1]);
        int i;
        for(i=match_info.rm_so;i<match_info.rm_eo;++i){
          putchar(s[i]);
        }
        printf("\n");
        return 0;
      }
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
  node *p;
  for(p=&head;p!=head.next;){
      //r->q--..->p
      node *q=head.next,*r=&head;
      while(q!=p){
        if(
          (q->time)<
          (q->next->time)
        ){
          swap(r,q,q->next);
          q=r->next;
        }else{
          r=q;
          q=q->next;
        }
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
    //before p->a->b->(b->next)
    p->next=b;
    a->next=b->next;
    b->next=a;
    //after  p->b->a->(b->next)
    return;
}
