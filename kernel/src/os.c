#include <common.h>
#include <amtrace.h>
#include <klib.h>
#include <vfs.h>
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

#define CURRENT_TEST queue_test
#define TEST_NAME(idx) TO_STRING(CURRENT_TEST) TO_STRING(idx)
#define TEST_REQUIREMENT() \
    void MACRO_CONCAT(MACRO_SELF(CURRENT_TEST),_init)(void); \
    MACRO_CONCAT(MACRO_SELF(CURRENT_TEST),_init)()

static void os_init() {
    pmm->init();
    kmt->init();
    dev->init();
    vfs->init();

    kmt->create(pmm->alloc(sizeof(task_t)),"shell1",mysh,"/dev/tty1");
    kmt->create(pmm->alloc(sizeof(task_t)),"shell2",mysh,"/dev/tty2");
    kmt->create(pmm->alloc(sizeof(task_t)),"shell3",mysh,"/dev/tty3");
    kmt->create(pmm->alloc(sizeof(task_t)),"shell4",mysh,"/dev/tty4");
    local_log("Os init finished\n");
}

static void hello() {
    //My printf is thread-safe
    printf("hello from CPU #%d\n",_cpu());
}

static void os_run() {
    _intr_write(0);
    hello();
    _intr_write(1);
    while(1)_yield();
}

static _Context *os_trap(_Event ev, _Context *context) {
    _intr_write(0);
    _Context *ret = context;
    //printf("trap");

    for(struct irq *handler=irq_guard.next;
        handler!=&irq_guard;
        handler=handler->next){
        if (handler->event == _EVENT_NULL || handler->event == ev.event) {
            _Context *next = handler->handler(ev, context);
            if (next) ret = next;
        }
    }
    Assert(ret!=NULL,"\nkmt_context_switch returns NULL\n");
    //printf("%x\n",ret);
    return ret;
}

static void os_on_irq(int seq, int event, handler_t handler) {
    irq_handler *prev=&irq_guard,*p=irq_guard.next;
    //prev->new->p
    while(1){
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
