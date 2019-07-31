#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <regex.h>
#include <stdio.h>
#include <time.h>
#include <sys/stat.h>
#include <fcntl.h>
int asprintf(char **strp,const char* fmt, ...);

#define Assert(cond, _fmt, ...) \
    do{ \
        if(!(cond)){ \
            fprintf(stderr, _fmt "\n" ##__VA_ARGS__); \
            exit(1); \
        } \
    }while(0)

#define err(...) \
    fprintf(stderr,__VA_ARGS__); \
    exit(1)

#define LEN(_array) \
    (sizeof(_array)/sizeof(_array[0]) )

typedef struct node{
    char *name;
    struct node *next;
    double time;
}node;
node head={
  .name="\1\0",
  .next=&head,
  .time=0
};//guard

double total=0.0;
regex_t name,num,exit_pat;
regmatch_t match_info;
void sort(void);
void display(void);
inline void swap(node*,node*,node*);
int main(int argc, char *argv[],char *envp[]) {
  //Check arguments
  Assert(argc>1,"spref: usage: spref PROG [ARGS]");
  //Create pipes
  int pipes[2];
  Assert(pipe(pipes)==0,"Build pipe failed!");
  //compile regexs
  Assert( (
    regcomp(&name,"^[a-zA-Z]*_*[a-zA-Z]*[0-9]*\\(",REG_EXTENDED) ||
    regcomp(&num,"<[0-9\\.]*>\n",REG_EXTENDED)  ||
    regcomp(&exit_pat,"exited with [0-9]* ",REG_EXTENDED) ) == 0,
          "Regexes compiled failed!\n");
  //Fork
  int pid=fork();
  Assert(pid>=0,"Fork failed!");
  if(pid==0){
    //Child process
    //Prepare file descriptors
    int out = open("/dev/null", O_RDWR|O_APPEND, 0600);
    int backup[2];
    if(out<0){
        err("Can not open /dev/null\nsee %s:%d for more help",__FILE__,__LINE__);
        //If can not open /dev/null,
        //you can comment this check and let
        //child process output
        //Or simply close them
        close(1);close(2);
    }else{
        backup[0]=dup(1);backup[1]=dup(2);
        dup2(out,1);dup2(out,2);
    }
    //Prepare new_argv[]
    char  *file;
    Assert(asprintf(&file,"/proc/%d/fd/%d",getppid(),pipes[1])>0,"No space for filename");
    const char *const flags[]={"-T","-o",file};
    const int new_argc=argc+LEN(flags);
    char **new_argv=(char**)malloc(sizeof(void*)*(new_argc+1));
    Assert(new_argv,"No space for new_argv");

    new_argv[0]="/usr/bin/strace";
    memcpy(new_argv+1           , flags,LEN(flags)*sizeof(void*));
    memcpy(new_argv+LEN(flags)+1,argv+1,      argc*sizeof(void*));
    //new_argv[new_argc] == argv[argc] == NULL
    Assert(new_argv[new_argc]==NULL,);

    execve("/usr/bin/strace",new_argv,envp);

    //Execve error
    fflush(stdout);
    fflush(stderr);
    dup2(backup[0],1);dup2(backup[1],2);
    printf("%s:%d Should not reach here!\n",__FILE__,__LINE__);
    fflush(stdout);
    return -1;
  }else{
    //Parent process
    dup2(pipes[0],0);
    char s[512];
    char call[20];
    double time_cost;
    time_t oldtime=0,newtime;
    while(fgets(s,sizeof(s),stdin)>0){
        if(time(&newtime)>oldtime){
            oldtime=newtime;
            sort();
            display();
        }
        if(regexec(&exit_pat,s,1,&match_info,0)!=REG_NOMATCH){
            sort();
            display();
            s[match_info.rm_eo]='\0';
            printf("%s :%s\n",argv[1],s+match_info.rm_so);
            return 0;
        }
        //Get name of syscall
        if(regexec(&name,s,1,&match_info,0)==REG_NOMATCH){
            continue;
        }
        strncpy(call, s+match_info.rm_so, match_info.rm_eo-match_info.rm_so );
        call[match_info.rm_eo-match_info.rm_so-1]='\0';
        //Get time of syscall
        if(regexec(&num,s,1,&match_info,0)==REG_NOMATCH){
            continue;
        }
        time_cost=atof(s+match_info.rm_so+1);
        //Record 
        node *p=&head,*q=NULL;
        do{
            q=p;
            p=p->next;
        }while(p!=&head && strcmp(p->name,call)!=0);
    
        if(strcmp(p->name,call)){
            //Not found
            //before q----->p
            //after  q-new->p
            q->next=(node*)malloc(sizeof(node));
            q->next->next=p;
            q=q->next;
            asprintf(&q->name,"%s",call);
            q->time=0.0;
        }
        q->time+=time_cost;
        total+=time_cost;
    }
    err("error with fgets");
    return -1;
  }
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
                //r->.->q--..->p
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
  printf("\033[2J\033[39m\033[49mSyscalls:\n");
  for(node *p=head.next;p!=&head;p=p->next){
    printf("%20s:%10lfs(%.2lf%%)\n",
            p->name,p->time,p->time*100/total);
  }
}
inline void swap(node *p,node *a,node *b){
    //before p->a->b->(b->next)
    p->next=b;
    a->next=b->next;
    b->next=a;
    //after  p->b->a->(b->next)
}
