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
        Assert(_intr_read()==0);
        printf("%c",((char*)arg)[0]);
        kmt->sem_wait(&echo_sem);
        _yield();
    }
}
void sem_test(void *arg){
    _intr_write(0);
    while(1){
        Assert(_intr_read()==0);
        for(volatile int i=0;i<100;++i);
        printf("~");
        kmt->sem_signal(&echo_sem);
        //printf("~");
        //kmt->sem_signal(&echo_sem);
        _yield();
    }
}
void idle(void *arg){
    while(1);
}

static void os_init() {
    pmm->init();
    kmt->init();
    dev->init();
    /*
    kmt->create(pmm->alloc(sizeof(task_t)),"idle1",idle,NULL);
    kmt->create(pmm->alloc(sizeof(task_t)),"idle2",idle,NULL);
    */
    kmt->create(pmm->alloc(sizeof(task_t)),"sem-test1",sem_test,"!");
    //kmt->create(pmm->alloc(sizeof(task_t)),"sem-test1",sem_test,"!");
    kmt->create(pmm->alloc(sizeof(task_t)),"echo-test:n",echo_test,"n");
    kmt->create(pmm->alloc(sizeof(task_t)),"echo-test:m",echo_test,"m");
    //kmt->create(pmm->alloc(sizeof(task_t)),"echo-test:s",echo_test,"s");
    //kmt->create(pmm->alloc(sizeof(task_t)),"echo-test:l",echo_test,"l");
    kmt->sem_init(&echo_sem,"echo-sem",10);
    log("Os init finished\n");
    //kmt->create(pmm->alloc(sizeof(task_t)), "print", echo_task, "tty1");
    //kmt->create(pmm->alloc(sizeof(task_t)), "print", echo_task, "tty2");
    //kmt->create(pmm->alloc(sizeof(task_t)), "print", echo_task, "tty3");
    //kmt->create(pmm->alloc(sizeof(task_t)), "print", echo_task, "tty4");
}

static void hello() {
    //My printf is thread-safe
    printf("nmsl from CPU #%d\n",_cpu());
}
inline void fill(uint8_t *p,int a,int b,int len){
    for(int i=0;i<len;++i){
        p[i]=(a+=b);
    }
}
inline void check(uint8_t *p,int a,int b,long long len){
    for(int i=0;i<len;++i){
        Assert(p[i]==(a+=b));
    }
}
void memory_test(){
    int a[100],b[100];
    long long len[100];
    void *p[100];
    for(int i=0;i<100;++i){
        len[i]=rand()*1LL;
        a[i]=rand();
        b[i]=rand();
        p[i]=pmm->alloc(len[i]);
        fill(p[i],a[i],b[i],len[i]);
    }
    for(int j=0;j<100;++j){
        int i=rand()%100;
        check(p[i],a[i],b[i],len[i]);
        pmm->free(p[i]);
        len[i]=rand()*1LL;
        if(i&1){
            len[i]+=(rand()*1LL)<<8;
        }
        a[i]=rand();
        b[i]=rand();
        p[i]=pmm->alloc(len[i]);
        fill(p[i],a[i],b[i],len[i]);
    }
    printf("[cpu%d] finish memory test.\n",_cpu());
    return;
}
static void os_run() {
    hello();
    _intr_write(0);
    memory_test();
    while (1) {
        _yield();
    }
}

static _Context *os_trap(_Event ev, _Context *context) {
    intr_close();
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
    Assert(ncli[_cpu()]==0,"%d",ncli[_cpu()]);
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
