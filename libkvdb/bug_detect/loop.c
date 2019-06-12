#include <stdio.h>
#include <stdlib.h>
#include "kvdb.h"
int main(){
    kvdb_t *db=malloc(sizeof(kvdb_t));
    kvdb_open(db,DB_FILE);
    srand(time(NULL));
    int i=rand();
    char key[30],value[30];
    while(++i,1){
        sprintf(key,"%d",i&0xf);
        sprintf(value,"%d",i);
        kvdb_put(db,key,value);
    }
    return 0;
}
