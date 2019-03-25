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
void show_free_list(void);
    show_free_list();
    int rorder[10],i;
    void *space[10];
    for(i=0;i<10;++i){
        int p=rand()%10,q=rand()%10;//shuffle
        int temp=rorder[p];
        rorder[p]=rorder[q];
        rorder[q]=temp;
        space[0]=pmm->alloc(rand()%100);
    }
    for(i=0;i<10;++i){
        free(space[rorder[i]]);//free in random order
    }
    show_free_list();

}

static void os_run() {
  hello();
  if(_cpu()==0){
      test();
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
