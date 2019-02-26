#include <stdio.h>
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
int digit_judge(char* s){
    while((*s)!='\0'){
        if(*s<'0'||*s>'9')return 0;
        ++s;
    }
    return 1;
}
#define maxn 100
char buf[maxn];
struct Node;
struct{
int :1;
}print_flag;
struct Proc{
    char* name;
    struct List* list;
}*info[50000];//TODO: dynamic map
struct List{
    struct Node *head,*tail;
};
struct Node{
    struct Proc** procp;
    struct Node* next;
};
typedef struct Proc Proc;
typedef struct List List;
typedef struct Node Node;
void add_sonpro(List* lp,pid_t ppid){
    if(lp->head==NULL){
        Node *tmp=malloc(sizeof(Node));
        lp->head=lp->tail=tmp;
        tmp->procp=&info[ppid];
        tmp->next=NULL;
    }else{
        lp->tail->next=malloc(sizeof(Node));
        lp->tail->next->procp=&info[ppid];
        lp->tail=lp->tail->next;
    }
}
void print_proc(Proc** proc){
    printf("%s(%d)\n",(*proc)->name,(int)(proc-info));
}
void maketree(char *dir){
    DIR *dp;
    struct dirent *entry;
    char filename[50],proname[50];
    FILE* fp=NULL;
    test(  ((dp = opendir(dir)) != NULL),  "Can not open /proc\n");
    test((chdir(dir)==0),"Can not cd to /proc");
    while((entry = readdir(dp)) != NULL) {
        if(digit_judge(entry->d_name)) {
            sprintf(filename,"%s%s",entry->d_name,"/status");
            test((fp=fopen(filename,"r"))!=NULL,"Can not open %s\n",filename);
            fscanf(fp,"Name:\t%s",proname);

            pid_t pid,ppid;
            while(fscanf(fp,"Pid:\t%d",&pid)!=1)fgets(buf,100,fp);
            info[pid]=malloc(sizeof(Proc));
            info[pid]->name=malloc(strlen(proname)+1);
            strcpy(info[pid]->name,proname);
            info[pid]->list=malloc(sizeof(List));
            info[pid]->list->head=info[pid]->list->tail=NULL;

            while(fscanf(fp,"PPid:\t%d",&ppid)!=1)fgets(buf,100,fp);
            if(ppid>0){add_sonpro((info[ppid]->list),pid);}
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
struct{
    char* arg;
    void(*handler)(void);
}arg_list={
    {"-V",main}
};
int main(int argc, char *argv[]) {
    int i;
    for (i = 0; i < argc; i++) {
        assert(argv[i]); // always true
        printf("argv[%d] = %s\n", i, argv[i]);
    }
    maketree("/proc");
    print_tree();
    return 0;
}
//Copy from https://stackoverflow.com/questions/8149569/scan-a-directory-to-find-files-in-c
