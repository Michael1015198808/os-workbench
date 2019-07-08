#include <common.h>
#include <klib.h>

task_t* get_cur(void){
    _intr_close();
    int cpu_id=_cpu();
    return currents[cpu_id];
    _intr_open();
}

void _putc(char);
//No lock, so can be called by gdb when dead-locked.
void print(char* s,int l){
    do{
        _putc(*s);
    }while(*(++s)&&(l--));
}
task_t *tasks[40];
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
    while(1);
}
void report(){
    asm volatile("nop;");
}
void report_if(int i){
    if(i){
        report();
    }
}
