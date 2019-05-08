#include <common.h>
#include <klib.h>

static void os_init() {
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
        int temp=rand()%POINTER_CNT;
        pmm->free(space[temp]);
        space[temp]=pmm->alloc(rand()&((1<<10)-1));
    }
    for(i=0;i<POINTER_CNT;++i){
        pmm->free(space[i]);
    }
}


void show(){
    void *space[10];
    int i;
    show_free_list();
    for(i=0;i<10;++i){
        space[i]=pmm->alloc(16);
    }
    show_free_list();
    pmm->free(space[5]);
    show_free_list();
    show_free_list();
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
  static pthread_mutex_lock trap_lk=PTHREAD_MUTEX_INITIALIZER;
  pthread_mutex_lock(&trap_lk);
  log("Someone calls os_trap");
  pthread_mutex_unlock(&trap_lk);
  while(1);
  //TODO: handle ev
  return context;
}

static void os_on_irq(int seq, int event, handler_t handler) {
}

MODULE_DEF(os) {
  .init   = os_init,
  .run    = os_run,
  .trap   = os_trap,
  .on_irq = os_on_irq,
};
