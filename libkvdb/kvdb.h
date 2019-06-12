#ifndef __KVDB_H__
#define __KVDB_H__
#include <stdint.h>
#include <pthread.h>


struct kvdb{
    int fd,hash;
    volatile int wr_acq,rd_cnt;
    pthread_rwlock_t lk;
};
typedef struct kvdb kvdb_t;

int kvdb_open(kvdb_t *db, const char *filename);
int kvdb_close(kvdb_t *db);
int kvdb_put(kvdb_t *db, const char *key, const char *value);
char *kvdb_get(kvdb_t *db, const char *key);
void kvdb_traverse(kvdb_t *db);

#endif
