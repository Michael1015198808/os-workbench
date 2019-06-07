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
    char key[3],value[100];
    kvdb_t *first=malloc(sizeof(kvdb_t));
    kvdb_open(first,DB_FILE);
    for(int i=0;i<10000000;++i){
        sprintf(key,"%d",i%10);
        sprintf(value,"%d",i);
        kvdb_put(first,key,value);
        usleep(100);
    }
    kvdb_close(first);
    return 0;
}
