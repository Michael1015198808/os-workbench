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

char *s[]={
    "STUID",
    "STUNAME"
};
#define LEN_S sizeof(s)/sizeof(s[0])
int main(){
    int i=0;
    char *values[LEN_S];
    kvdb_t first;
    kvdb_open(&first,DB_FILE);
    while(1){
        for(int i=0;i<LEN_S;++i){
            values[i]=kvdb_get(&first,s[i]);
        }
        //To make output flexible
        for(int i=0;i<LEN_S;++i){
            printf("%s: ",s[i]);
            puts(values[i]);
        }
        for(int i=0;i<LEN_S;++i){
            free(values[i]);
        }
        usleep(10000);
    }
    kvdb_close(&first);
    return 0;
}
