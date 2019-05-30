//No lock, so can be called by gdb when dead-locked.
#include <common.h>
#include <klib.h>
void _putc(char);
void print(char* s){
    do{
        _putc(*s);
    }while(*(++s));
}
task_t *tasks[20];
int tasks_cnt;
void show_attr(){
    int i;
    printf(",%d",(int)tasks[0]->attr);
    for(i=1;i<tasks_cnt;++i){
        printf(",%d",(int)tasks[i]->attr);
    }
    _putc('\n');
}
void show_tasks(){
    printf("%p",tasks[0]);
    for(int i=1;i<tasks_cnt;++i){
        printf(",%p",tasks[i]);
    }
    _putc('\n');
}
void infinite_loop(void){
    asm volatile("jmp 0");
}
