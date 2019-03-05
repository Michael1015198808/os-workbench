#include <game.h>

void init_screen(void);
void init(void);
void swap_pixel(void);
void draw_cursor(int);
//void read_key();
pixel color[GRID_NUM][GRID_NUM];
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
  draw_str("Move with arrow keys",0,0,2,0x3fff00);
  draw_str("Select grid with space key",0,16,2,0x3fff00);
  draw_str("Swap the tiles to put the colors in order!",0,32,2,0x3fff00);
  draw_cursor(1);
  while (1) {
    int key=read_key();
    if(key&0x8000){
      draw_grid(cursor_locat);
      switch(key^0x8000){
        case _KEY_SPACE:
          if(color[cursor_x][cursor_y].alpha==1)break;
          choosen[choosen_idx][0]=cursor_x;
          choosen[choosen_idx][1]=cursor_y;
          draw_circle(cursor_locat,SIDE,0xffffff,0x00000000);
          if(choosen_idx==0){choosen_idx=1;}
          else{choosen_idx=0;swap_pixel();}
          break;
        case _KEY_RIGHT:
          ++cursor_x;if(cursor_x>=GRID_NUM)cursor_x-=GRID_NUM;break;
        case _KEY_LEFT:
          --cursor_x;if(cursor_x<0)cursor_x+=GRID_NUM;break;
        case _KEY_DOWN:
          ++cursor_y;if(cursor_y>=GRID_NUM)cursor_y-=GRID_NUM;break;
        case _KEY_UP:
          --cursor_y;if(cursor_y<0)cursor_y+=GRID_NUM;break;
        default:
          break;
      }
      //printf("%d,%d\n",cursor_x,cursor_y);
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



void init(void){
  //pixel_t rd=rand(),ld=rand(),ru=rand(),lu=rand();
  pixel rd,ld,ru,lu;
  rd.val=0x00000000;//Right Down
  ld.val=0x00ff0000;//Left  Down
  ru.val=0x0000ff00;//Right Up
  lu.val=0x000000ff;//Left  Up
  for(int x=0;x<GRID_NUM;++x){
    pixel left=gradient(ld,lu,x);
    pixel right=gradient(rd,ru,x);
    for (int y=0;y<GRID_NUM;++y){
      color[x][y]=gradient(left,right,y);
    }
  }
  color[0][0].alpha=1;//Use alpha=1
  color[7][7].alpha=1;//to fix
  color[0][7].alpha=1;//specific
  color[7][0].alpha=1;//grids
  for(int i=0;i<100;++i){
      //swap times
      int j=rand()%36,k=rand()%36;
      while(color[j/6][j%6].alpha==1)j=rand()%36;
      while(color[k/6][k%6].alpha==1)k=rand()%36;
      pixel temp=color[j/6][j%6];
      color[j/6][j%6]=color[k/6][k%6];
      color[k/6][k%6]=temp;
  }
  for (int x = 0; x<GRID_NUM; x ++) {
    for (int y = 0; y<GRID_NUM; y++) {
      draw_grid(x,y);
      //mono_rect((x+MARGIN) * SIDE*3, (y+MARGIN) * SIDE*3, SIDE*3, SIDE*3, color[x][y].val);
    }
  }
}
pixel gradient(pixel a,pixel b,int i){
    pixel temp;
    temp.val=0;
    temp.r=((b.r-a.r)*i)/GRID_NUM+a.r;
    temp.g=((b.g-a.g)*i)/GRID_NUM+a.g;
    temp.b=((b.b-a.b)*i)/GRID_NUM+a.b;
    return temp;
}
void draw_cursor(int mode){
// 0 for wipe
// 1 for draw
  mono_rect((cursor_x+MARGIN) * SIDE*3+SIDE, (cursor_y+MARGIN) * SIDE*3+SIDE, SIDE, SIDE, mode==1?0xffffff:color[cursor_x][cursor_y].val);
}
void swap_pixel(void){
#define choosen_color(_idx) color[choosen[_idx][0]][choosen[_idx][1]]
  pixel temp=choosen_color(0);
  choosen_color(0)=choosen_color(1);
  choosen_color(1)=temp;
  draw_grid(choosen[0][0],choosen[0][1]);
  draw_grid(cursor_x,cursor_y);
}
