#include "kvdb.h"
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>

volatile int max[4];
volatile int writable=0,tot_cnt=0;
pthread_mutex_t cnt_lk=PTHREAD_MUTEX_INITIALIZER;
void *test_write(void *arg){
    void **args=arg;
    kvdb_t *db= args[0];
    int base=args[1];
    while(!writable);
    for(int i=0;i<100;++i){
        char key[5],val[20];
        sprintf(key,"%d",400*base+i);
        sprintf(val,"%d",i);
        kvdb_put(db,key,val);
        max[base]=i;
        printf("[%d]%d\n",base,i);
    }
    return NULL;
}
void *test_read(void *arg) {
    void **args=arg;
    kvdb_t *db = args[0];
    uintptr_t base = (uintptr_t)args[1];
    char key_str[5],*val,check[5];
    uintptr_t cnt=0;
    while(1){
        for(int i=0;i<max[base];++i,++cnt){
            sprintf(key_str,"%d",400*base+i);
            sprintf(check,"%d",i);
            val=kvdb_get(db,key_str);
            if(val==NULL || strcmp(check,val)){
                asm volatile("int $3");
            }
            free(val);
        }
        usleep(rand()&0xff);
        if(!rand()&0xf){
            usleep(0xffff);
        }
        if(max[base]==99)break;
    }
    pthread_mutex_lock(&cnt_lk);
    tot_cnt+=cnt;
    pthread_mutex_unlock(&cnt_lk);
    return NULL;
}

#define THREADS 64
#define ARGS 2
#define panic(fmt,...) \
    fprintf(stderr, __FILE__ ":%d " fmt,__LINE__, ##__VA_ARGS__)
int main(int argc, char *argv[]) {
    kvdb_t *db = malloc(sizeof(kvdb_t));
    uintptr_t args[THREADS][ARGS];

    if(db == NULL) { panic("malloc failed. \n"); return 1; } 
    if(kvdb_open(db, DB_FILE)) { panic("cannot open. \n"); return 1; }
 
    pthread_t pt[THREADS];
    for(int i = 0; i < 4; ++i ){
        args[i][0]=(uintptr_t)db;
        args[i][1]=i&3;
        pthread_create(&pt[i], NULL, test_write, args[i]);
    }
    for(int i = 4; i < THREADS; i ++) {
        args[i][0]=(uintptr_t)db;
        args[i][1]=i&3;
        pthread_create(&pt[i], NULL,  test_read, args[i]);
    }

    usleep(0xfff);
    writable=1;
    for(int i = 0; i < THREADS; i ++) {
        pthread_join(pt[i], NULL);
    }

    if(kvdb_close(db)) { panic("cannot close. \n"); return 1; }

    printf("%ld\n",tot_cnt);
    return 0;
}
