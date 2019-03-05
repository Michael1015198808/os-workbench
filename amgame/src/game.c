#include <game.h>

void init_screen(void);
void splash(void);
void init(void);
void swap_pixel(void);
void draw_cursor(int);
//void read_key();
union pixel{
  uint32_t val;
  struct{
    uint32_t b:8,g:8,r:8,alpha:8;
    //It seems alpha is useless
  };
};
typedef union pixel pixel;
pixel color[8][8];
pixel gradient(pixel,pixel,int);
int choosen[2][2],choosen_idx=0;
int cursor_x=0,cursor_y=0;
int main() {
  // Operating system is a C program
  srand(uptime(NULL));
  printf("Hello World from " __ISA__ " program!\n");
  _ioe_init();
  init_screen();
  init();
  splash();
  draw_str("Hello, world",0,0,1,0x3fff00);
  while (1) {
    int key=read_key();
    if(key&0x8000){
      draw_cursor(0);
      switch(key^0x8000){
        case _KEY_SPACE:
          choosen[choosen_idx][0]=cursor_x;
          choosen[choosen_idx][1]=cursor_y;
          if(choosen_idx==0){choosen_idx=1;}
          else{choosen_idx=0;swap_pixel();}
          break;
        case _KEY_DOWN:
          ++cursor_x;break;
        case _KEY_UP:
          --cursor_x;break;
        case _KEY_LEFT:
          --cursor_y;break;
        case _KEY_RIGHT:
          ++cursor_y;break;
        default:
          break;
      }
      printf("%d,%d\n",cursor_x,cursor_y);
      draw_cursor(1);
    }
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
  rd.val=0x00000000;//Right Down
  ld.val=0x00ff0000;//Left  Down
  ru.val=0x0000ff00;//Right Up
  lu.val=0x000000ff;//Left  Up
  for(int x=0;x<8;++x){
    pixel left=gradient(ld,lu,x);
    pixel right=gradient(rd,ru,x);
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
void draw_cursor(int mode){
// 0 for wipe
// 1 for draw
  mono_rect((cursor_x+MARGIN) * SIDE*3+SIDE, (cursor_y+MARGIN) * SIDE*3+SIDE, SIDE, SIDE, mode==1?0xffffff:color[cursor_x][cursor_y].val);
}
