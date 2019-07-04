#include <common.h>
#include <klib.h>

#ifndef NO_TEST
void move_esp(int depth){
    int large_variable[10];
    (void)large_variable;
    if(depth>0){
        move_esp(depth-1);
    }
}
void smash_test(void *idle){
    while(1){
        move_esp(10);
    }
}
void smash_test_init(void){
    kmt->create(pmm->alloc(sizeof(task_t)),"smash-test",smash_test,NULL);
}
#endif
