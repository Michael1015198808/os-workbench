#include <stdio.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>
#define test(_con,...) \
do{\
    if(!(_con)){\
        fprintf(stderr,__VA_ARGS__);\
        assert(0);\
    }\
}while(0)

int digit_judge(char*);
int (*cmp)(char*,char*);
int num(char* s1,char* s2);
int alpha(char* s1,char* s2);
#define maxn 100
char buf[maxn];
struct Node;
struct{
enum {numeric,alphabeta}Sort;
int show_pids:1;
}print_flag={
    alphabeta,
    0
};
struct Proc{
    char* name;//The name of the Proc
    struct Proc *son;//The first son of the Proc
    //struct Proc *sson;//For speed up
    struct Proc *bro;//The list of son of Proc's father
}*info[50000];//TODO: dynamic map
//Proc
// ^
// |
//info[pid]
// ^
// |
//Node--next->Node->Node
// ^                 ^
// |                 |
//head              tail
//|-------List-------|   name
//|-------------Proc---------|
typedef struct Proc Proc;
typedef struct List List;
typedef struct Node Node;

void add_sonpro(Proc* pp,pid_t pid){
    if(pp->son==NULL){
        pp->son=info[pid];
    }else{
        Proc *l=pp->son,*r=l->bro;
        while(r!=NULL&&cmp(r->name,info[pid]->name)){
            l=r;
            r=l->bro;
        };
        info[pid].bro=l->bro;
        l->bro=info[pid];
    }
}

void print_proc(Proc** proc){
    if(print_flag.show_pids){
        printf("%s(%d)\n",(*proc)->name,(int)(proc-info));
    }else{
        printf("%s\n",(*proc)->name);
    }
}
void init_pid(int pid){
    test(info[pid]=malloc(sizeof(Proc)),"malloc size for Proc failed!");
    info[pid]->son=info[pid]->bro=NULL;
}

void make_tree(void){
    DIR *dp;
    struct dirent *entry;
    char filename[50],proname[50];
    FILE* fp=NULL;
    test(  ((dp = opendir("/proc")) != NULL),  "Can not open /proc\n");
    test((chdir("/proc")==0),"Can not cd to /proc");
    while((entry = readdir(dp)) != NULL) {
        if(digit_judge(entry->d_name)) {
            sprintf(filename,"%s%s",entry->d_name,"/status");
            test((fp=fopen(filename,"r"))!=NULL,"Can not open %s\n",filename);
            fscanf(fp,"Name:\t%s",proname);

            pid_t pid,ppid;
            while(fscanf(fp,"Pid:\t%d",&pid)!=1)fgets(buf,100,fp);
            if(info[pid]==NULL){init_pid(pid);}
            info[pid]->name=malloc(strlen(proname)+1);
            strcpy(info[pid]->name,proname);

            while(fscanf(fp,"PPid:\t%d",&ppid)!=1)fgets(buf,100,fp);
            if(ppid>0){
                if(info[ppid]==NULL){init_pid(ppid);}
                add_sonpro(info[ppid],pid);
            }
            fclose(fp);
        }
    }
    closedir(dp);
}

void print_tree(void){
    Proc** pp=&info[1];
    Node *head=(*pp)->list->head,*tail=(*pp)->list->tail;
    print_proc(pp);
    if(head==NULL)return;
    while(head!=tail){
        print_proc(head->procp);
        head=head->next;
    }
}
void version(void);
void numeric_sort(void);
void show_pids(void);
void bug_fix_log(void);
struct{
    char* arg_name;//char** with int len can't KISS
    void(*handler)(void);
}arg_list[]={
    {"-V",version},
    {"--version",version},
    {"-n",numeric_sort},
    {"--numeric-sort",numeric_sort},
    {"-p",show_pids},
    {"--show-pids",show_pids},
    {"-log",bug_fix_log},
    {"\0",NULL}//To make format more beautiful
};
int main(int argc, char *argv[]) {
    int i;
    cmp=alpha;
    for (i = 1; i < argc; i++) {
        int j;
        for(j=0;j<sizeof(arg_list)/sizeof(arg_list[0]);++j){
            if(!strcmp(arg_list[j].arg_name,argv[i])){
                arg_list[j].handler();
                break;
            }
        }
        if(j>=sizeof(arg_list)/sizeof(arg_list[0])){
            printf("\33[1;31mUnsupported arg(s):%s\33[0m\n",argv[i]);
        }
    }
    //puts("args handled");
    make_tree();
    print_tree();
    return 0;
}
//Copy from https://stackoverflow.com/questions/8149569/scan-a-directory-to-find-files-in-c
void version(void){
    puts("pstree 0.1");
    puts("Copyright (C) 2019-2019 Michael Yan");
    exit(0);
}
int digit_judge(char* s){
    while((*s)!='\0'){
        if(!isdigit(*s))return 0;
        //Use library function to improve robustness
        ++s;
    }
    return 1;
}
int num(char* s1,char* s2){
    int a,b;
    sscanf(s1,"%d",&a);
    sscanf(s2,"%d",&b);
    return a<b;
}
int alpha(char* s1,char* s2){
    while((*s1==*s2)&&(*s1!='\0')){
        ++s1;++s2;
    }
    if(s1==s2)return 0;
    else{
        return *s1<*s2;
    }
}
void numeric_sort(void){
    cmp=num;
}
void show_pids(void){
    print_flag.show_pids=1;
}
void bug_fix_log(void){
#define BUG(_msg) puts("\33[1;31mbug:" #_msg "\33[0m")
#define FIX(_msg) puts("\33[1;31mfixed by" #_msg "\33[0m")
    BUG(There may be process that ppid>pid);
    FIX(check the initilization of info);
    exit(0);
}

