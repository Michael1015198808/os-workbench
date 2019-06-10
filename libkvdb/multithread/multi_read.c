#include "kvdb.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

void *special_test(void *arg){
    void **args=arg;
    kvdb_t *db= args[0];
    int cnt=0;
    while(1){
        char key[5],val[20];
        sprintf(key,"%d",cnt%100);
        sprintf(val,"%d",cnt);
        kvdb_put(db,key,val);
    }
}
void *test(void *arg) {
    void **args=arg;
    kvdb_t *db = args[0];
    uintptr_t key_val = (uintptr_t)args[1];
    char key_str[5],*val;
    sprintf(key_str,"%d",key_val);
    for(int i=1;i!=0;++i){
        val=kvdb_get(db,key_str);
        if(val==NULL){
            asm volatile("int $3");
        }
        free(val);
        //usleep(1000);
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
    for(int i = 0; i < THREADS; i ++) {
        args[i][0]=(uintptr_t)db;
        args[i][1]=(100/32)*i+1;
        pthread_create(&pt[i], NULL, test, args[i]);
    }

    for(int i = 0; i < THREADS; i ++) {
        pthread_join(pt[i], NULL);
    }

    if(kvdb_close(db)) { panic("cannot close. \n"); return 1; }
    system("systemctl hibernate");

    return 0;
}
