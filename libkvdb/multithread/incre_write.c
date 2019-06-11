#include "kvdb.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

volatile int max[2];
void *write(void *arg){
    void **args=arg;
    kvdb_t *db= args[0];
    int base=args[1];
    for(int i=0;i<50;++i){
        char key[5],val[20];
        sprintf(key,"%d",50*base+i);
        sprintf(val,"%d",i);
        kvdb_put(db,key,val);
        max[base]=i;
    }
    return NULL;
}
void *test(void *arg) {
    void **args=arg;
    kvdb_t *db = args[0];
    uintptr_t base = (uintptr_t)args[1];
    char key_str[5],*val,check[5];
    while(1){
        for(int i=0;i<max[base];++i){
            sprintf(key_str,"%d",50*base+i);
            sprintf(check,"%d",i);
            val=kvdb_get(db,key_str);
            if(val==NULL || strcmp(check,val)){
                asm volatile("int $3");
            }
            free(val);
        }
        if(max[base]==49)break;
    }
    return NULL;
}

#define THREADS 32
#define ARGS 2
#define panic(fmt,...) \
    fprintf(stderr, __FILE__ ":%d " fmt,__LINE__, ##__VA_ARGS__)
int main(int argc, char *argv[]) {
    kvdb_t *db = malloc(sizeof(kvdb_t));
    uintptr_t args[THREADS][ARGS];

    if(db == NULL) { panic("malloc failed. \n"); return 1; }
 
    if(kvdb_open(db, DB_FILE)) { panic("cannot open. \n"); return 1; }
 
    pthread_t pt[THREADS];
    for(int i = 0; i < 2; ++i ){
        args[i][0]=(uintptr_t)db;
        args[i][1]=i&1;
        pthread_create(&pt[i], NULL, write, args[i]);
    }
    for(int i = 2; i < THREADS; i ++) {
        args[i][0]=(uintptr_t)db;
        args[i][1]=i&1?50:0;
        pthread_create(&pt[i], NULL,  read, args[i]);
    }

    for(int i = 0; i < THREADS; i ++) {
        pthread_join(pt[i], NULL);
    }

    if(kvdb_close(db)) { panic("cannot close. \n"); return 1; }

    return 0;
}
