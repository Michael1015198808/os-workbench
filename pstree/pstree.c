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
int is_digit(char* s){
    while((*s)!='\0'){
        if(*s<'0'||*s>'9')return 0;
        ++s;
    }
    return 1;
}
void printdir(char *dir, int depth){
    DIR *dp;
    struct dirent *entry;
    char statp[50],proname[50];
    FILE* fp=NULL;
    test(  ((dp = opendir(dir)) != NULL),  "Can not open /proc\n");
    test((chdir(dir)==0),"Can not cd to /proc");
    while((entry = readdir(dp)) != NULL) {
        if(is_digit(entry->d_name)) {
            strcpy(statp,entry->d_name);
            strcat(statp,"/status");
            //fprintf(statp+strlen(statp),"%s/status",entry_>d_name);
            test((fp=fopen(statp,"r"))!=NULL,"Can not open %s\n",statp);
            fscanf(fp,"Name:\t%s",proname);
            pid_t pid;
            while(fscanf(fp,"Pid:\t%d",&pid)!=1)fgets(fp);
            printf("pid:%d\t",pid);
            puts(proname);
        }
    }
    closedir(dp);
}

int main(){
    printdir("/proc",0);
    return 0;
}
//Copy from https://stackoverflow.com/questions/8149569/scan-a-directory-to-find-files-in-c
/*int main(int argc, char *argv[]) {
  printf("Hello, World!\n");
  int i;
  for (i = 0; i < argc; i++) {
    assert(argv[i]); // always true
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  printf("%d\n",getpid());
  system("ls /proc");
  getchar();//Give me time to find it in /proc
  assert(!argv[argc]); // always true
  return 0;
}*/
