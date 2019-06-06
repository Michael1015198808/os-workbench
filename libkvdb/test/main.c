#include <assert.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <inttypes.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <regex.h>
#include <setjmp.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "kvdb.h"

#define safe_call(Sentence) \
    { \
    int ret=(Sentence); \
        if(ret){ \
            fprintf(stderr,__FILE__ ":%d %s returns %d(%x)!\n",__LINE__, #Sentence, ret, ret); \
        }; \
    }

#define log(fmt,...) \
    printf("%d: " fmt, __LINE__ ##__VA_ARGS__);

#define Assert(cond,...) \
    do { \
      if(!(cond)){ \
        asm volatile("nop;nop;"); \
/* Make it easier to set breakpoints.*/\
        printf("\33[1;31m"); \
        log("Assertion " #cond " fails!\n" __VA_ARGS__); \
        printf("\33[0m"); \
        exit(0); \
      } \
    }while(0)

int main(){
    int fd=open("test.txt",O_RDWR);
    Assert(fd>=0);
    struct stat st;
    fstat(fd,&st);
    uint8_t *disk = mmap(NULL, st.st_size+0x10, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    puts(disk);
    while(1){
        int idx,c;
        scanf("%d %c",&idx,&c);
        disk[idx]=c;
    }
    return 0;
}
