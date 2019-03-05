#include <game.h>

void init_screen();
void splash();
void init();
//void read_key();
typedef uint32_t pixel_t;
pixel_t color[8][8];
int main() {
  // Operating system is a C program
  srand(uptime(NULL));
  printf("Hello World from " __ISA__ " program!\n");
  _ioe_init();
  init_screen();
  init();
  splash();
  while (1) {
    read_key();
  }
  return 0;
}

/*void read_key() {
  _DEV_INPUT_KBD_t event = { .keycode = _KEY_NONE };
  #define KEYNAME(key) \
    [_KEY_##key] = #key,
  static const char *key_names[] = {
    _KEYS(KEYNAME)
  };
  _io_read(_DEV_INPUT, _DEVREG_INPUT_KBD, &event, sizeof(event));
  if (event.keycode != _KEY_NONE && event.keydown) {
    puts("Key pressed: ");
    puts(key_names[event.keycode]);
    puts("\n");
  }
}*/

int w, h;

void init_screen() {
  _DEV_VIDEO_INFO_t info = {0};
  _io_read(_DEV_VIDEO, _DEVREG_VIDEO_INFO, &info, sizeof(info));
  w = info.width;
  h = info.height;
}

void mono_rect(int x, int y, int w, int h, uint32_t color) {
  uint32_t pixels[w * h]; // WARNING: allocated on stack
  _DEV_VIDEO_FBCTL_t event = {
    .x = x, .y = y, .w = w, .h = h, .sync = 1,
    .pixels = pixels,
  };
  for (int i = 0; i < w * h; i++) {
    pixels[i] = color;
  }
  _io_write(_DEV_VIDEO, _DEVREG_VIDEO_FBCTL, &event, sizeof(event));
}

void splash() {
#define MARGIN 1
  for (int x = 0; x<8; x ++) {
    for (int y = 0; y<8; y++) {
      mono_rect((x+MARGIN) * SIDE*4, (y+MARGIN) * SIDE*4, SIDE*4, SIDE*4, color[x][y]); // white
    }
  }
}
void init(void){
  pixel_t rd=rand(),ld=rand(),ru=rand(),lu=rand();
  for(int x=0;x<8;++x){
    pixel_t left=((ld-lu)*x)/8+lu;
    pixel_t right=((rd-ru)*x)/8+ru;
    for (int y=0;y<8;++y){
        pixel_t current=((right-left)*y)/8+left;
        color[x][y]=current;
    }
  }
}
