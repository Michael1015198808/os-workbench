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
    _intr_write(0);
    while(1){
        printf("%c",((char*)arg)[0]);
        kmt->sem_wait(&echo_sem);
        _yield();
    }
}
void sem_test(void *arg){
    while(1){
        for(volatile int i=0;i<100;++i);
        printf("~");
        kmt->sem_signal(&echo_sem);
        //printf("~");
        //kmt->sem_signal(&echo_sem);
        _yield();
    }
}
void idle(void *arg){
    while(1){
        for(volatile int i=0;i<10000;++i)_yield();
    };
}
void echo_task(void *name) {
  device_t *tty = dev_lookup(name);
  while (1) {
    char line[128], text[128];
    sprintf(text, "(%s) $ ", name); tty->ops->write(tty, text);
    int nread = tty->ops->read(tty, 0, line, sizeof(line));
    line[nread - 1] = '\0';
    sprintf(text, "Echo: %s.\n", line); tty_write(tty, text);
  }
}

static void os_init() {
    pmm->init();
    kmt->init();
    dev->init();
    kmt->create(pmm->alloc(sizeof(task_t)),"idle1",idle,NULL);
    kmt->create(pmm->alloc(sizeof(task_t)),"idle2",idle,NULL);
    kmt->create(pmm->alloc(sizeof(task_t)),"echo_task",echo_task,NULL);
    /*
    kmt->create(pmm->alloc(sizeof(task_t)),"sem-test1",sem_test,"!");
    kmt->create(pmm->alloc(sizeof(task_t)),"sem-test2",sem_test,"!");
    kmt->create(pmm->alloc(sizeof(task_t)),"echo-test:n",echo_test,"n");
    kmt->create(pmm->alloc(sizeof(task_t)),"echo-test:m",echo_test,"m");
    kmt->create(pmm->alloc(sizeof(task_t)),"echo-test:s",echo_test,"s");
    kmt->create(pmm->alloc(sizeof(task_t)),"echo-test:l",echo_test,"l");
    kmt->sem_init(&echo_sem,"echo-sem",10);
    */
    local_log("Os init finished\n");
    //kmt->create(pmm->alloc(sizeof(task_t)), "print", echo_task, "tty1");
    //kmt->create(pmm->alloc(sizeof(task_t)), "print", echo_task, "tty2");
    //kmt->create(pmm->alloc(sizeof(task_t)), "print", echo_task, "tty3");
    //kmt->create(pmm->alloc(sizeof(task_t)), "print", echo_task, "tty4");
}

static void hello() {
    //My printf is thread-safe
    printf("nmsl from CPU #%d\n",_cpu());
}

static void os_run() {
    hello();
    _intr_write(1);
    while (1) {
        _yield();
    }
}

static _Context *os_trap(_Event ev, _Context *context) {
    intr_close();
    report_if(ncli[_cpu()]!=1);
    _Context *ret = context;

    for(struct irq *handler=irq_guard.next;
        handler!=&irq_guard;
        handler=handler->next){
        if (handler->event == _EVENT_NULL || handler->event == ev.event) {
            _Context *next = handler->handler(ev, context);
            if (next) ret = next;
        }
    }
    intr_open();
    //Assert(ncli[_cpu()]==0,"%d",ncli[_cpu()]);
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
    prev->next->next=p;

    prev->next->seq=seq;
    prev->next->event=event;
    prev->next->handler=handler;
}

MODULE_DEF(os) {
  .init   = os_init,
  .run    = os_run,
  .trap   = os_trap,
  .on_irq = os_on_irq,
};
