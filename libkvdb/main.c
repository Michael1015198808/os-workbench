#include "kvdb.h"
#include <stdio.h>
#include <stdlib.h>

#define safe_call(Sentence) \
    { \
    int ret=(Sentence); \
        if(ret){ \
            fprintf(stderr,__FILE__ ":%d %s",__LINE__, ##Sentence); \
        }; \
    }
int main(){
    safe_call(printf("nmsl\n"));
    return 0;
}
