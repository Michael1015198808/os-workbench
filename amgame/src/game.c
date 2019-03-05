#include <game.h>

void init_screen();
void splash();
void init();
//void read_key();
union pixel{
  uint32_t val;
  struct{
    uint32_t alpha:8,r:8,g:8,b:8;
  };
};
typedef union pixel pixel;
pixel color[8][8];
pixel gradient(pixel,pixel,int);
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
      mono_rect((x+MARGIN) * SIDE*3, (y+MARGIN) * SIDE*3, SIDE*3, SIDE*3, color[x][y].val);
      /*printf("x:%d,y:%d,pixel:%x\n",x,y,color[x][y]);
      int key;
      while ((key = read_key()) == _KEY_NONE);*/
    }
  }
}
void init(void){
  //pixel_t rd=rand(),ld=rand(),ru=rand(),lu=rand();
  pixel rd,ld,ru,lu;
  rd.val=0x00000000;
  ld.val=0x00ff0000;
  printf("ld.val:%x,alpha:%x,r:%x,g:%x,b:%x\n",ld.val,ld.alpha,ld.r,ld.g,ld.b);
  while (read_key() == _KEY_NONE);
  mono_rect(0,0, SIDE*3, SIDE*3, ld.val);
  ru.val=0x0000ff00;
  lu.val=0x000000ff;
  for(int x=0;x<8;++x){
    pixel left=gradient(ld,lu,x);
    pixel right=gradient(rd,ru,x);
    if(x==0)printf("left:%x,right:%x\n",left.val,right.val);
    for (int y=0;y<8;++y){
      color[x][y]=gradient(left,right,y);
    }
  }
}
pixel gradient(pixel a,pixel b,int i){
    pixel temp;
    temp.val=0;
    temp.r=((b.r-a.r)*i)/8+a.r;
    temp.g=((b.g-a.g)*i)/8+a.g;
    temp.b=((b.b-a.b)*i)/8+a.b;
    return temp;
}
