#include <game.h>

int main() {
  srand(uptime());
  printf("Hello World from " __ISA__ " program!\n");
  _ioe_init();
  init_screen();
  init();
  while (1) {
    print_instr();
    operate(next_key());
  }
  return 0;
}






