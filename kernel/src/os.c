#include <common.h>
#include <klib.h>

static void os_init() {
  pmm->init();
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
    void *space[10];
    int i;
    for(i=0;i<10;++i){
        int temp=rand()%100;
        space[i]=pmm->alloc(temp);
    }
    for(i=0;i<1000;++i){
        int temp=rand()%10;
        pmm->free(space[temp]);
        space[temp]=pmm->alloc(rand()%100);
    }
    for(i=0;i<10;++i){
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
void big_size_test(void){
    void *p[100];
    int i;
    for(i=0;i<100;++i){
        p[i]=pmm->alloc(4 << 10);
    }
    show_free_pages();
    for(i=0;i<100;++i){
        pmm->free(p[i]);
    }
    show_free_pages();
}
static void os_run() {
  hello();
  if(_cpu()==0){
      //test();
      big_size_test();
      //show();
  }
  _intr_write(1);
  while (1) {
    _yield();
  }
}

static _Context *os_trap(_Event ev, _Context *context) {
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
