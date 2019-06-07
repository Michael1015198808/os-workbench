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

int main(){
    int i=0;
    char s[10],*value;
    kvdb_t *first=malloc(sizeof(kvdb_t));
    while(1){
        for(int i=0;i<10;++i){
            sprintf(s,"%d",i);
        }
        puts(value=kvdb_get(first,s));
        free(value);
    }
    kvdb_close(first);
    return 0;
}
