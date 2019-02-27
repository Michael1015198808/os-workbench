#include <stdio.h>
#include <stdint.h>
#include <ctype.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <dirent.h>
#include <string.h>
#include <sys/stat.h>

#define IGNORE_PRO_EXIT
//Ignore the processes that are not existing when open
//comment it to allow error report
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
unsigned int show_pids:1;
}print_flag={
    0
};
struct Proc{
    pid_t pid;
    uint8_t cnt;//Used to merge
    char* name;//The name of the Proc
    struct Proc *son;//The first son of the Proc
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
        if(cmp(pp->son,info[pid])>0){
            info[pid]->bro=pp->son;
            pp->son=info[pid];
            return;
        }
        if(print_flag.show_pids==0&&pp->son->cnt!=0&&cmp(pp->son,info[pid])==0){
            ++pp->son->cnt;
            return;
        }
        Proc *l=pp->son,*r=l->bro;
        while(r!=NULL&&cmp(r,info[pid])<0){
            l=r;
            r=l->bro;
        };
        if(print_flag.show_pids==0&&r!=NULL&&r->cnt!=0&&cmp(r,info[pid])==0){
            ++r->cnt;
            return;
        }
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
    info[pid]->name=NULL;
    info[pid]->cnt=0;
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
#ifdef IGNORE_PRO_EXIT
            if((fp=fopen(filename,"r"))==NULL)continue;
#else
            test((fp=fopen(filename,"r"))!=NULL,"Can not open %s\n",filename);
#endif
            fscanf(fp,"Name:\t%s",proname);

            pid_t pid,ppid;
            while(fscanf(fp,"Pid:\t%d",&pid)!=1)fgets(buf,100,fp);
            if(info[pid]==NULL){init_pid(pid);}
            if(info[pid]->name==NULL)info[pid]->name=malloc(strlen(proname)+1);
            strcpy(info[pid]->name,proname);

            while(fscanf(fp,"PPid:\t%d",&ppid)!=1)fgets(buf,100,fp);
            if(ppid>0){
                if(info[ppid]==NULL){init_pid(ppid);}
                add_sonpro(info[ppid],pid);
            }
            fclose(fp);

            DIR* tasks;
            struct dirent* task_entry;
            sprintf(filename,"%s%s",entry->d_name,"/task");
            test(  ((tasks= opendir(filename)) != NULL),  "Can not open /proc/%s\n",filename);
            while((task_entry = readdir(tasks)) != NULL) {if(digit_judge(task_entry->d_name)) {
                sprintf(filename,"%s%s%s%s",entry->d_name,"/task/",task_entry->d_name,"/status");
#ifdef IGNORE_PRO_EXIT
                if((fp=fopen(filename,"r"))==NULL)continue;
#else
                test((fp=fopen(filename,"r"))!=NULL,"Can not open %s\n",filename);
#endif
                fscanf(fp,"Name:\t%s",proname);
                pid_t ppid=pid;
                pid_t pid;
                while(fscanf(fp,"Pid:\t%d",&pid)!=1)fgets(buf,100,fp);
                if(pid!=ppid){
                    if(info[pid]==NULL){init_pid(pid);}
                    if(info[pid]->name==NULL){
                        info[pid]->name=malloc(strlen(proname)+3);
                        sprintf(info[pid]->name,"{%s}",proname);
                        info[pid]->cnt=1;
                    }
                    add_sonpro(info[ppid],pid);
                    fclose(fp);
                }
            }
        }}
    }
    closedir(dp);
}
//static unsigned int bar_len=strlen("│");
//since the bar is more than 1 byte
int blank_len[10]={},bar_exist[10]={};
int depth=-1;
void print_tree(const Proc const *p,int is_first){

//Due to different coding like UTF-8, the output maybe different
//I've tried my best to make every line in output correctly inter-
//sect. If it's not so in judge computer, PLZ contact me.

    int len=0;
#define output(...) (len+=printf(__VA_ARGS__))
//For format, DO NOT use PRINTF, use output to track the indent length, and don't use its return value!
//-bar_len is used to omit the rightmost bar, because it's replaced by other kinds
#define delete_bar() bar_exist[depth]=0;
    if(is_first==0){
    //judge if this node shares the same line with its father
    //if not,print the pattern for indent
        int i,j;
        for(i=0;i<depth;++i){
            for(j=0;j<blank_len[i];++j){
                putchar(' ');
            }
            printf("%s",bar_exist[i]!=0?"│":" ");
        }
        for(j=0;j<blank_len[depth];++j){
            putchar(' ');
        }
        if(p->bro!=NULL){printf("├─");}
        else{printf("└─");delete_bar();}
    }else if(p->pid!=1){
        if(p->bro!=NULL){printf("─┬─");}
        else{printf("───");delete_bar();}
    }
    //print itself
    if(p->cnt>1){
        output("%d*[",p->cnt);
    }
    output("%s",p->name);
    if(print_flag.show_pids){
        output("(%d)",p->pid);
    }
    if(p->cnt>1){
        output("]");
    }

    if(p->son==NULL){putchar('\n');return;}


    blank_len[++depth]=len;
    bar_exist[depth]=p->son->bro!=NULL;
    //print its sons
    print_tree(p->son,1);
    //The first son shares the same line, so do not need to print pattern
    Proc* current=p->son->bro;
    while(current!=NULL){
        bar_exist[depth]=current->bro!=NULL;
        print_tree(current,0);
        current=current->bro;
    }
    --depth;
    return;
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
    print_tree(info[1],1);
    putchar('\n');
    return 0;
}
//The oldest version is copied from https://stackoverflow.com/questions/8149569/scan-a-directory-to-find-files-in-c
//I only refer 2 library functions from the primal code
void version(void){
    puts("pstree 1.0");
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
    if(strcmp(p1->name,p2->name)){
        return p1->pid>p2->pid?1:-1;
    }else{
        return 0;
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
#define FIX(_msg) puts("\33[1;34mfixed by " #_msg "\33[0m")
    BUG("There may be process that ppid>pid");
    FIX("checking the initilization of info");
    BUG("│ takes more than 1 byte, wipe it cause ? symbol in bash");
    FIX("Using pointer operation and sprintf to wipe it");
    exit(0);
}

