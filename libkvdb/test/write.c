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
    kvdb_t *first=malloc(sizeof(kvdb_t));
    char s1[100],s2[100];
    kvdb_open(first, "first.db");
    while(1){
        scanf("%s:%s",s1,s2);
        kvdb_put(first,s1,s2);
    }
    kvdb_close(first);
    return 0;
}
