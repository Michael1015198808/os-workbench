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
    char s[10];
    char *values[10];
    kvdb_t first;
    kvdb_open(&first,"./first.db");
    while(1){
        for(int i=0;i<10;++i){
            sprintf(s,"%d",i);
            printf("%d: ",i);
            puts(values[i]=kvdb_get(&first,s));
        }
        //To make output flexible
        for(int i=0;i<10;++i){
            puts(values[i]);
        }
        for(int i=0;i<10;++i){
            free(values[i]);
        }
        for(volatile int i=0;i<1000000;++i);
    }
    kvdb_close(&first);
    return 0;
}
