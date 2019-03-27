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
    void *space[100];
    int i;
    for(i=0;i<100;++i){
        space[i]=pmm->alloc(rand()%((1<<10)-1));
    }
    for(i=0;i<1000;++i){
        int temp=rand()%10;
        pmm->free(space[temp]);
        space[temp]=pmm->alloc(rand()&((1<<10)-1));
    }
    for(i=0;i<100;++i){
        pmm->free(space[i]);
    }
}
#define test_ptr_nr 128
#define MAX_CPU 2
static void* test_ptrs[MAX_CPU][test_ptr_nr];
void alloc_test() {

  for (int i = 0; i < test_ptr_nr; i++) {
    test_ptrs[_cpu()][i] = pmm->alloc(1 << 12);
  }
  for (int i = 0; i < test_ptr_nr; i++) {
    pmm->free(test_ptrs[_cpu()][i]);
  }
  printf("CPU #%d pass test 1\n",_cpu());
  return;

  for (int i = 0; i < test_ptr_nr; i++) {
    test_ptrs[_cpu()][i] = pmm->alloc(1 << 2);
  }
  for (int i = 0; i < test_ptr_nr; i++) {
    pmm->free(test_ptrs[_cpu()][test_ptr_nr - i - 1]);
  }
  printf("CPU #%d pass test 2\n",_cpu());

  for (int i = 0; i < test_ptr_nr; i++) {
    test_ptrs[_cpu()][i] = pmm->alloc(1 << 14);
  }
  for (int i = 0; i < test_ptr_nr; i++) {
    pmm->free(test_ptrs[_cpu()][i]);
  }
  printf("CPU #%d pass test 3\n",_cpu());

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
  alloc_test();
  show_free_list();
  //show();
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
