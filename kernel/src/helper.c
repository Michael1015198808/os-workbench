//No lock, so can be called by gdb when dead-locked.
#include <common.h>
#include <klib.h>
void _putc(char);
void print(char* s){
    do{
        _putc(*s);
    }while(*(++s));
}
task_t **tasks;
int tasks_cnt;
void show_attr(){
    int i;
    printf(",%d",tasks[0]->attr);
    for(i=1;i<tasks_cnt;++i){
        printf(",%d",tasks[i]->attr);
    }
    putchar('\n');
}
