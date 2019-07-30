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
int asprintf(char **strp,const char*,...);
/*
Support args in forms like '-np' or '-n -p'
and assert properly.
Two method to handle when scan some process that isn't alive.
Perfect format(At least perfect on my PC)
merge process with same names.
 */
#define IGNORE_PRO_EXIT
//Ignore the processes that are not existing when open
//comment it to allow error report

#define RED "\33[1;31m"
#define ORI "\33[0m"
#define LEN(_array) ( sizeof(_array)/sizeof(_array[0]) )

#define Assert(_con,_fmt,...) \
do{\
    if(!(_con)){\
        fprintf(stderr,RED "Assertion failed:\nLine: %d" _fmt, __LINE__ ,##__VA_ARGS__);\
        assert(0);\
    }\
}while(0)

#define safe_printf(_str , ...) \
    Assert(snprintf( \
            _str, sizeof(_str)-1, __VA_ARGS__ \
                )>0, "process's name is too long")


int digit_judge(char*);
struct Proc{
    pid_t pid;
    uint8_t cnt;//Used to merge
    char* name;//The name of the Proc
    struct Proc *son;//The first son of the Proc
    struct Proc *bro;//The list of son of Proc's father
}*info[1<<16];
//     bro     bro    bro     bro
//Proc*-->Proc*-->...-->Proc*|-->NULL(forbidden)
//  ^                        |
//  |son                     |
//  |                        |
//Proc*                      |
typedef struct Proc Proc;

struct{
    int (*cmp)(const Proc*,const Proc*);
    unsigned int show_pids:1;
}print_flag={};

void add_sonpro(Proc* pp,pid_t pid){
    int (*const cmp)(const Proc*,const Proc*)=print_flag.cmp;
    if(pp->son==NULL){
        pp->son=info[pid];
    }else{
        //Insert sort
        if(cmp(pp->son,info[pid])>0){
            info[pid]->bro=pp->son;
            pp->son=info[pid];
            return;
        }else if(pp->son->cnt!=0 && cmp(pp->son,info[pid])==0){
            ++pp->son->cnt;
            return;
        }
        Proc *l=pp->son,*r=l->bro;
        while(r && cmp(r,info[pid])<0){
            l=r;
            r=l->bro;
        }
        if(r && r->cnt!=0 && cmp(r,info[pid])==0){
            ++r->cnt;
            return;
        }
        info[pid]->bro=l->bro;
        l->bro=info[pid];
    }
}
void init_pid(int pid){
    Assert(info[pid]=malloc(sizeof(Proc)),"malloc size for Proc failed!");
    *info[pid]=(Proc){
        .son=NULL,
        .bro=NULL,
        .pid=pid,
        .name=NULL,
        .cnt=0,
    };
}

void make_tree(void){
    DIR *dp;
    struct dirent *entry;
    char filename[50],proname[50];
    FILE* fp=NULL;
    Assert( (dp = opendir("/proc")) , "Can not open /proc\n");
    Assert( chdir("/proc")==0 ,"Can not cd to /proc\n");
    while((entry = readdir(dp)) ){
        if(digit_judge(entry->d_name)){
            pid_t pid,ppid;

            safe_printf(filename,"%s/status",entry->d_name);
#ifdef IGNORE_PRO_EXIT
            if((fp=fopen(filename,"r"))==NULL)continue;
#else
            Assert((fp=fopen(filename,"r")),"Can not open %s\n",filename);
#endif

            fscanf(fp,"Name:\t%s",proname);

#define MAXN 100
            char buf[MAXN];
            while(fscanf(fp,"Pid:\t%d",&pid)!=1)fgets(buf,MAXN,fp);
            if(info[pid]==NULL){init_pid(pid);}
            if(info[pid]->name==NULL)asprintf(&info[pid]->name,"%s",proname);

            while(fscanf(fp,"PPid:\t%d",&ppid)!=1)fgets(buf,MAXN,fp);
            if(ppid>0){
                if(info[ppid]==NULL){init_pid(ppid);}
                add_sonpro(info[ppid],pid);
            }
            fclose(fp);

            DIR* tasks;
            struct dirent* task_entry;
            safe_printf(filename,"%s/task",entry->d_name);
            Assert( (tasks= opendir(filename)) ,  "Can not open /proc/%s\n",filename);
            while( (task_entry = readdir(tasks)) ){ if(digit_judge(task_entry->d_name)){
                safe_printf(filename,"%s/task/%s/status",entry->d_name,task_entry->d_name);
#ifdef IGNORE_PRO_EXIT
                if((fp=fopen(filename,"r"))==NULL)continue;
#else
                Assert(fp=fopen(filename,"r") ,"Can not open %s\n",filename);
#endif
                fscanf(fp,"Name:\t%s",proname);
                pid_t ppid=pid;
                pid_t pid;
                while(fscanf(fp,"Pid:\t%d",&pid)!=1)fgets(buf,MAXN,fp);
                if(pid!=ppid){
                    if(info[pid]==NULL){init_pid(pid);}
                    if(info[pid]->name==NULL){
                        asprintf(&info[pid]->name,"{%s}",proname);
                        info[pid]->cnt=1;
                    }
                    add_sonpro(info[ppid],pid);
                    fclose(fp);
                }
            }}
        }
    }
    closedir(dp);
}
#define MAX_DEP 20
int blank_len[MAX_DEP]={},bar_exist[MAX_DEP]={};
int depth=-1;

void print_tree(const Proc *const p,int is_first){
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
        for(int i=0;i<=depth;++i){
            printf("%*s",blank_len[i]," ");
            if(i<depth)
                printf("%s",bar_exist[i]!=0?"│ ":"  ");
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

    blank_len[++depth]=len+1;
    bar_exist[depth]= (p->son->bro!=NULL);
    //print its sons
    print_tree(p->son,1);
    //The first son shares the same line, so do not need to print pattern
    for(Proc* cur=p->son->bro;
            cur;
            cur=cur->bro){
        bar_exist[depth]=cur->bro!=NULL;
        print_tree(cur,0);
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
}two_dash_arg_list[]={
    {"--version",version},
    {"--numeric-sort",numeric_sort},
    {"--show-pids",show_pids},
    {"--log",bug_fix_log},
    {"\0",NULL}//To make format more beautiful
};
struct{
    char arg_name;
    void(*handler)(void);
}single_dash_arg_list[]={
    {'V',version},
    {'n',numeric_sort},
    {'p',show_pids}
};
//ares with -- here

int arg_matched(const char* const arg){
    if(arg[0]=='-'){
        if(arg[1]=='-'){
            //args with --
            for(int j=0;j<LEN(two_dash_arg_list);++j){
                if(!strcmp(two_dash_arg_list[j].arg_name,arg)){
                    two_dash_arg_list[j].handler();
                    return 1;
                }
            }
        }else{
            //args with -
            int j=1;
            for(int k=1;arg[k];++k){
                for(j=0;j<LEN(single_dash_arg_list);++j){
                    if(single_dash_arg_list[j].arg_name==arg[k]){
                        single_dash_arg_list[j].handler();
                        break;
                    }
                }
                if(j==LEN(single_dash_arg_list)) return 0;
            }
            return 1;
        }
    }

    //Successful matches returns half-way
    return 0;
}

int main(int argc, char *argv[]) {
    int alpha(const Proc*,const Proc*);
    print_flag.cmp=alpha;
    for(int i=1; i<argc; ++i){
        if(!arg_matched(argv[i])){
            printf("Unsupported arg(s):" RED "%s\n" ORI,argv[i]);
            exit(0);
        }
    }
    make_tree();
    print_tree(info[1],1);
    return 0;
}

//The oldest version is copied from https://stackoverflow.com/questions/8149569/scan-a-directory-to-find-files-in-c
//2 library functions refer to the primal code
void version(void){
    puts("pstree 2.0");
    puts("Copyright (C) 2019-2019 Michael Yan");
    exit(0);
}
int digit_judge(char* s){
    //Use library function to improve robustness
    while(isdigit(*s)){
        ++s;
    }
    return *s=='\0';
}
int alpha(const Proc* p1,const Proc* p2){
    return strcmp(p1->name,p2->name);
}
int num(const Proc* p1,const Proc* p2){
    if(print_flag.show_pids || strcmp(p1->name,p2->name) ){
        return p1->pid - p2->pid;
    }
    return 0;
}
void numeric_sort(void){
    print_flag.cmp=num;
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

