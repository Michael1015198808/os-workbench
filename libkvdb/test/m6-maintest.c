#include "kvdb.h"
#include <stdlib.h>
#include <stdio.h>

void *test(void *_db) {
  kvdb_t *db = _db;
// code: 
#include "test.h"
// #define EXIT
#ifdef EXIT
  exit(1);
#endif
  return NULL;
}

#define THREADS 4

int main(int argc, char *argv[]) {
  kvdb_t *db = malloc(sizeof(kvdb_t));
  if(db == NULL) { panic("malloc failed. \n"); return 1; }

  if(kvdb_open(db, argv[1])) { panic("cannot open. \n"); return 1; }

  pthread_t pt[THREADS];
  for(int i = 0; i < THREADS; i ++) {
    pthread_create(&pt[i], NULL, test, db);

  }
  for(int i = 0; i < THREADS; i ++) {
    pthread_join(pt[i], NULL);
  }

  if(kvdb_close(db)) { panic("cannot close. \n"); return 1; }

  return 0;
}