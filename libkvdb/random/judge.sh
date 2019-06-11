#!/bin/bash
i=1;
while true;
do
    ./gen.out 1> in.c 2>out2;
    gcc -g -lpthread -DDB_FILE=\"db/rand.db\" -I.. -L.. -m64 in.c -o test-64 -lkvdb-64;
    gcc -g -lpthread -DDB_FILE=\"db/rand.db\" -I.. -L.. -m32 in.c -o test-32 -lkvdb-32;
    LD_LIBRARY_PATH=.. ./test-64 >out1;
    if ! diff out1 out2;
        then break;
    fi;

    if ! LD_LIBRARY_PATH=.. ./test-32 >out1;
        then break;
    fi;

    diff out1 out2;
    echo "pass " $i "-th test";
    (( i=i+1 ));
done;
