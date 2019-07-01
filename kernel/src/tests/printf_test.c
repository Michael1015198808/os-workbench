#include <common.h>
#include <klib.h>

#ifndef NO_TEST
void printf_test_init(void){
    return;
}
void printf_test(void *dummy){
    while(1){
        printf("Hello!%c\n",((char*)dummy)[0]);
    }
}

#endif
