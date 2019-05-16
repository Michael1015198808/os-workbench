#include <common.h>
#include <klib.h>

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
        //printf("loop\n");
        printf("!");
        kmt->sem_signal(&echo_sem);
        kmt->sem_signal(&echo_sem);
        //while(1);
        _yield();
    }
}
static void os_init() {
    pmm->init();
    kmt->init();
    dev->init();
    kmt->create(pmm->alloc(sizeof(task_t)),"echo-test",echo_test,"n");
    kmt->create(pmm->alloc(sizeof(task_t)),"echo-test",echo_test,"m");
    kmt->create(pmm->alloc(sizeof(task_t)),"echo-test",echo_test,"s");
    kmt->create(pmm->alloc(sizeof(task_t)),"echo-test",echo_test,"l");
    kmt->create(pmm->alloc(sizeof(task_t)),"sem-test",sem_test,"!");
    kmt->create(pmm->alloc(sizeof(task_t)),"sem-test",sem_test,"!");
    kmt->sem_init(&echo_sem,"echo-sem",0);
    extern void kmt_test(void);
    //kmt_test();
    log("Os init finished\n");
    //kmt->create(pmm->alloc(sizeof(task_t)), "print", echo_task, "tty1");
    //kmt->create(pmm->alloc(sizeof(task_t)), "print", echo_task, "tty2");
    //kmt->create(pmm->alloc(sizeof(task_t)), "print", echo_task, "tty3");
    //kmt->create(pmm->alloc(sizeof(task_t)), "print", echo_task, "tty4");
}

//volatile static _Thread_local int cnt=0;
static void hello() {
  /*for (const char *ptr = "Hello from CPU #"; *ptr; ptr++) {
    _putc(*ptr);
  }
  _putc("12345678"[_cpu()]); _putc('\n');*/
  //My printf is thread-safe
  //while(1){
    //printf("Hello from CPU #%d for the %d-th time\n",_cpu(),++cnt);
    printf("nmsl from CPU #%d\n",_cpu());
  //}
}

static void os_run() {
  /*if(_cpu()==0){
      printf("%d\n",*(int*)0);
  }*/
  log("Intr%d\n",_intr_read());
  hello();
  while (1) {
    _yield();
  }
}

int switch_flag[5];
pthread_mutex_t irq_lk;
static _Context *os_trap(_Event ev, _Context *context) {
    pthread_mutex_lock(&irq_lk);
    _Context *ret = context;
    switch_flag[_cpu()]=0;

    for(struct irq *handler=irq_guard.next;
        handler!=&irq_guard;
        handler=handler->next){
        if (handler->event == _EVENT_NULL || handler->event == ev.event) {
            //log("Call one handler\n");
            //log("%d\n",ret);
            _Context *next = handler->handler(ev, context);
            if (next) ret = next;
        }
    }
    pthread_mutex_unlock(&irq_lk);
    //log("ret%p\n",ret);
    if(ret==NULL){
        log("\n%d\n",switch_flag[_cpu()]);
    };
    return ret;
}


static void os_on_irq(int seq, int event, handler_t handler) {
    pthread_mutex_lock(&irq_lk);
    log("%p\n",handler);
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
    pthread_mutex_unlock(&irq_lk);
}
void irq_test(){
    irq_handler *p=irq_guard.next;
    while(p){
        ((void(*)(void))p->handler)();
        p=p->next;
    }
}
/*void echo_task(void *name) {
  device_t *tty = dev_lookup(name);
  while (1) {
    char line[128], text[128];
    sprintf(text, "(%s) $ ", name); tty_write(tty, text);
    int nread = tty->ops->read(tty, 0, line, sizeof(line));
    line[nread - 1] = '\0';
    sprintf(text, "Echo: %s.\n", line); tty_write(tty, text);
  }
}*/

MODULE_DEF(os) {
  .init   = os_init,
  .run    = os_run,
  .trap   = os_trap,
  .on_irq = os_on_irq,
};
