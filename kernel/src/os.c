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
/*void test(){
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
}*/

void test(){
    int *space[100];
    int num[100];
    int i;
    for(i=0;i<100;++i){
        num[i] = rand()%((1<<5)-1);
        space[i]=pmm->alloc(num[i]);
        for(int j=0;j < num[i]/sizeof(int);j++)
          space[i][j] = j;
    }
    for(i=0;i<1000;++i){
        int temp=rand()%10;
        for(int j=0;j < num[temp]/sizeof(int);j++)
          assert(space[temp][j] == j);
        pmm->free(space[temp]);
        num[temp] = rand()%((1<<5)-1);
        space[temp]=pmm->alloc(num[temp]);
        for(int j=0;j < num[temp]/sizeof(int);j++)
          space[temp][j] = j;
    }
    for(i=0;i<100;++i){
        for(int j=0;j < num[i]/sizeof(int);j++)
          assert(space[i][j] == j);
        pmm->free(space[i]);
    }
    printf("test: I finished\n");
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
  hello();
  test();
  show_free_list();
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
