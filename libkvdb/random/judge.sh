#!/bin/bash
./gen.out 1> in.c 2>out2
gcc -g -DDB_FILE=\"db/rand.db\" -I.. -L.. -m64 in.c -o test-64 -lkvdb-64
gcc -g -DDB_FILE=\"db/rand.db\" -I.. -L.. -m32 in.c -o test-32 -lkvdb-32
LD_LIBRARY_PATH=.. ./test-64 >out1
diff out1 out2
LD_LIBRARY_PATH=.. ./test-32 >out1
diff out1 out2
