#include <common.h>
#include <amtrace.h>
#include <klib.h>
#include <devices.h>

typedef struct irq{
    int seq,event;
    handler_t handler;
    struct irq *next;
}irq_handler;

void guard(void){
    Assert(0,"Guard should not be called!\n");
}

static irq_handler irq_guard={
    .seq=INT_MIN,
    .event=-1,
    .handler=(handler_t)guard,
    .next=&irq_guard
};

sem_t echo_sem;
void echo_test(void *arg){
    while(1){
        printf("%c",((char*)arg)[0]);
        kmt->sem_wait(&echo_sem);
        _yield();
    }
}
void sem_test(void *arg){
    while(1){
        printf("~");
        kmt->sem_signal(&echo_sem);
        _yield();
    }
}
void idle(void *arg){
    while(1){
        for(volatile int i=0;i<10000;++i)_yield();
    };
}

void intr_reading(void *idle){
    while(1){
        int i=_intr_read();
        _putc('0'+i);
        _putc('\n');
    }
}

#define CURRENT_TEST multithread_test
#define TEST_NAME(idx) TO_STRING(CURRENT_TEST) TO_STRING(idx)
#define TEST_REQUIREMENT() \
    void MACRO_CONCAT(MACRO_SELF(CURRENT_TEST),_init)(void); \
    MACRO_CONCAT(MACRO_SELF(CURRENT_TEST),_init)()

void main_dead_loop(void){
    int main();
    asm volatile("jump %0"::"=g"(main));
}
static void os_init() {
    pmm->init();
    kmt->init();
    dev->init();
    TEST_REQUIREMENT();
#undef CURRENT_TEST
#define CURRENT_TEST semaphore_test
    /*
    TEST_REQUIREMENT();
    kmt->create(pmm->alloc(sizeof(task_t)),"sem-test1",sem_test,"!");
    kmt->create(pmm->alloc(sizeof(task_t)),"sem-test2",sem_test,"!");
    kmt->create(pmm->alloc(sizeof(task_t)),"echo-test:n",echo_test,"n");
    kmt->create(pmm->alloc(sizeof(task_t)),"echo-test:m",echo_test,"m");
    kmt->create(pmm->alloc(sizeof(task_t)),"echo-test:s",echo_test,"s");
    kmt->create(pmm->alloc(sizeof(task_t)),"echo-test:l",echo_test,"l");
    kmt->sem_init(&echo_sem,"echo-sem",10);
    kmt->create(pmm->alloc(sizeof(task_t)),"reading",intr_reading,NULL);
    kmt->create(pmm->alloc(sizeof(task_t)),"idle1",idle,NULL);
    kmt->create(pmm->alloc(sizeof(task_t)),"idle2",idle,NULL);
    kmt->create(pmm->alloc(sizeof(task_t)),"shell1",mysh,"tty1");
    kmt->create(pmm->alloc(sizeof(task_t)),"shell2",mysh,"tty2");
    kmt->create(pmm->alloc(sizeof(task_t)),"shell3",mysh,"tty3");
    kmt->create(pmm->alloc(sizeof(task_t)),"shell4",mysh,"tty4");
    */
    local_log("Os init finished\n");
}

static void hello() {
    //My printf is thread-safe
    printf("nmsl from CPU #%d\n",_cpu());
}

static void os_run() {
    hello();
    _intr_write(1);
    while(1);
}

static _Context *os_trap(_Event ev, _Context *context) {
    _intr_write(0);
    _Context *ret = context;

    for(struct irq *handler=irq_guard.next;
        handler!=&irq_guard;
        handler=handler->next){
        if (handler->event == _EVENT_NULL || handler->event == ev.event) {
            _Context *next = handler->handler(ev, context);
            if (next) ret = next;
        }
    }
    Assert(ret!=NULL,"\nkmt_context_switch returns NULL\n");
    return ret;
}


static void os_on_irq(int seq, int event, handler_t handler) {
    irq_handler *prev=&irq_guard,*p=irq_guard.next;
    //prev->new->p
    while(p){
        if(p->seq>seq||p==&irq_guard)break;
        prev=p;
        p=p->next;
    }
    prev->next=new(irq_handler);
    prev=prev->next;

    prev->seq=seq;
    prev->event=event;
    prev->handler=handler;
    prev->next=p;
}

MODULE_DEF(os) {
  .init   = os_init,
  .run    = os_run,
  .trap   = os_trap,
  .on_irq = os_on_irq,
};
