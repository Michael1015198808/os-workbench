#include <common.h>
#include <klib.h>

#define printf_test_init
void printf_test(void *dummy){
    while(1){
        printf("Hello!%c\n",((char*)dummy)[0]);
    }
}
