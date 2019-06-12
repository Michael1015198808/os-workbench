#include "kvdb.h"
int main(){
    kvdb_t test;
    printf("%p",kvdb_get(&test,"STUID"));
    return 0;
}
