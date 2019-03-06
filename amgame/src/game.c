#include <game.h>

int main() {
  srand(uptime());
  printf("Hello World from " __ISA__ " program!\n");
  _ioe_init();
  init_screen();
  init();
  draw_str("Move with arrow keys\n"
           "Select grid with space key\n"
           "Swap the tiles to put the colors in order!\n",0,0,2,0x3fff00);
  draw_str("Press h for hint",0,h-2*8,2,0x3fff00);
  while (1) {
    update();
  }
  return 0;
}






