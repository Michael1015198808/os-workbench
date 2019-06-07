#include "kvdb.h"
int main(){
    kvdb_t first;
    kvdb_open(&first,"./first.db");
    kvdb_traverse(&first);
    kvdb_close(&first);
}
