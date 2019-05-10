#include <common.h>
#include <klib.h>

typedef struct irq{
    int event,seq;
    handler_t handler;
    struct irq *next;
}irq_handler;
static irq_handler *handlers=NULL;
void guard(void){
    Assert(0,"Guard should not be called!\n");
}

static inline void handler_init(void){
    handlers=new(irq_handler);
    handlers->event=-1;
    handlers->seq=0;
    handlers->handler=(handler_t)guard;
    handlers->next=NULL;
}
static void os_init() {
  handler_init();
  pmm->init();
  kmt->init();
  _vme_init(pmm->alloc, pmm->free);
  dev->init();
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
  hello();
  _intr_write(1);
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
      log("Call one handler\n");
      _Context *next = handler->handler(ev, context);
      if (next) ret = next;
    }
  }
  //log("Someone calls os_trap");
  pthread_mutex_unlock(&trap_lk);
  return ret;
}


static void os_on_irq(int seq, int event, handler_t handler) {
    Assert(handlers==NULL);
    irq_handler *prev=NULL,*p=handlers;
//prev->new->p
    while(p){
        if(p->seq>seq||(p->seq==seq&&p->event>=event))break;
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
