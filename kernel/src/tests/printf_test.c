#include <common.h>
#include <klib.h>
void printf_test(void *dummy){
    while(1){
        printf("Hello!%c\n",((char*)dummy)[0]);
    }
}
