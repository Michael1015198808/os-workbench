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
void check_puts(char *s){
    if(s)puts(s);
}

int main(){
    kvdb_t *first=malloc(sizeof(kvdb_t));
    kvdb_open(first, "first.db");
    kvdb_put(first,"username","Michael Yan");
    check_puts(kvdb_get(first,"username"));
    kvdb_put(first,"username","Michael Yan NJU");
    check_puts(kvdb_get(first,"username"));
    kvdb_close(first);
    return 0;
}
