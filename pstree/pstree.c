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
#define maxn 100
char buf[maxn];
struct Node;
struct{
int show_pids:1;
}print_flag={
    0
};
struct Proc{
    pid_t pid;
    char* name;//The name of the Proc
    struct Proc *son;//The first son of the Proc
    //struct Proc *sson;//For speed up
    struct Proc *bro;//The list of son of Proc's father
}*info[50000];//TODO: dynamic map
//     bro     bro    bro     bro
//Proc*-->Proc*-->...-->Proc*|-->NULL(forbidden)
//  ^                        |
//  |son                     |
//  |                        |
//Proc*                      |
typedef struct Proc Proc;
typedef struct List List;
typedef struct Node Node;

int (*cmp)(Proc*,Proc*);
int num(Proc*,Proc*);
int alpha(Proc*,Proc*);

void add_sonpro(Proc* pp,pid_t pid){
    if(pp->son==NULL){
        pp->son=info[pid];
    }else{
        if(!cmp(pp->son,info[pid])){
            info[pid]->bro=pp->son;
            pp->son=info[pid];
            return;
        }
        Proc *l=pp->son,*r=l->bro;
        while(r!=NULL&&cmp(r,info[pid])){
            l=r;
            r=l->bro;
        };
        info[pid]->bro=l->bro;
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
    info[pid]->pid=pid;
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

void print_tree(const Proc const *p,const char* pattern){

    char new_pattern[200];
    sprintf(new_pattern,"%s%*s",pattern,(int)strlen(p->name)+4,"│");

    //print itself
    if(p->pid!=1){
        printf("%.*s",(int)strlen(pattern)-1,pattern);
        if(p->bro!=NULL){printf(" ├─");}
        else{printf(" └─");}
    }
    printf("%s",p->name);
    if(print_flag.show_pids){
        printf("(%d)",p->pid);
    }

    if(p->son==NULL){putchar('\n');return;}
    if(p->son->bro==NULL){
        printf("───");
    }else{
        printf("─┬─");
    }
    Proc* current=p->son;
    //print its sons
    while(current!=NULL){
        print_tree(current,new_pattern);
        current=current->bro;
    }
    if(p->bro!=NULL){
            printf("%s",pattern);
            if(p->bro->bro==NULL){
                printf("└─");
            }else{
                printf("├─");
            }
        }
    return;
}
//
//pattern name --- son
//pattern          bro
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
    print_tree(info[1],"");
    putchar('\n');
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
int alpha(Proc* p1,Proc* p2){
    return strcmp(p1->name,p2->name);
}
int num(Proc* p1,Proc* p2){
    return p1->pid<p2->pid;
}
void numeric_sort(void){
    cmp=num;
}
void show_pids(void){
    print_flag.show_pids=1;
}
void bug_fix_log(void){
#define BUG(_msg) puts("\33[1;31mbug:" #_msg "\33[0m")
#define FIX(_msg) puts("\33[1;31mfixed by " #_msg "\33[0m")
    BUG(There may be process that ppid>pid);
    FIX(check the initilization of info);
    exit(0);
}

