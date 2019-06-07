#include "kvdb.h"
int main(){
    kvdb_t first;
    kvdb_open(&first,DB_FILE);
    kvdb_traverse(&first);
    kvdb_close(&first);
}
