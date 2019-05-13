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
},*handlers=&irq_guard;

static void os_init() {
  pmm->init();
  kmt->init();
  dev->init();
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
    printf("Hello from CPU #%d\n",_cpu());
  //}
}
void test(){
#define POINTER_CNT 1000
    void *space[POINTER_CNT];
    int i;
    for(i=0;i<POINTER_CNT;++i){
        space[i]=pmm->alloc(rand()%((1<<10)-1));
    }
    for(i=0;i<1000;++i){
        int idx=rand()%POINTER_CNT,len=rand()&((1<<10)-1);
        pmm->free(space[idx]);
        space[idx]=pmm->alloc(len);
    }
    for(i=0;i<POINTER_CNT;++i){
        pmm->free(space[i]);
    }
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

static _Context *os_trap(_Event ev, _Context *context) {
  static pthread_mutex_t trap_lk=PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_lock(&trap_lk);
  _Context *ret = context;
  for(struct irq *handler=handlers;handler!=NULL;handler=handler->next){
    if (handler->event == _EVENT_NULL || handler->event == ev.event) {
      //log("Call one handler\n");
      _Context *next = handler->handler(ev, context);
      if (next) ret = next;
    }
  }
  pthread_mutex_unlock(&trap_lk);
  log("ret%p",ret);
  return ret;
}


static void os_on_irq(int seq, int event, handler_t handler) {
    Assert(handlers!=(void*)NULL,"Handler haven't initialized");
    irq_handler *prev=handlers,*p=handlers->next;
//prev->new->p
    while(p){
        if(p->seq>seq||p==handlers)break;
        prev=p;
        p=p->next;
    }
    prev->next=new(irq_handler);
    prev->next->next=p;

    prev->next->seq=seq;
    prev->next->event=event;
    prev->next->handler=handler;
}
void irq_test(){
    irq_handler *p=handlers->next;
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
