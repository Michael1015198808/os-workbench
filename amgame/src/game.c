#include <game.h>

void init_screen(void);
void init(void);
void swap_pixel(void);
int choosen[2],choosen_idx=0;
uint8_t idx[GRID_NUM][GRID_NUM];
int cursor_x=0,cursor_y=0,print_flag=1;
int main() {
  // Operating system is a C program
  uint32_t old_time=uptime(NULL),new_time=0;
  srand(old_time);
  printf("Hello World from " __ISA__ " program!\n");
  _ioe_init();
  init_screen();
  init();
  draw_str("Move with arrow keys\n"
           "Select grid with space key\n"
           "Swap the tiles to put the colors in order!\n",0,0,2,0x3fff00);
  draw_str("Press h for hint",0,h-2*8,2,0x3fff00);
  while (1) {
    old_time=new_time;
    if(print_flag==1){
        draw_cursor(1);
    }else{
      int help_x,help_y;
      if(choosen_idx==1){
        help_x=choosen[0];
        help_y=choosen[1];
      }else{
        help_x=cursor_x;
        help_y=cursor_y;
      }
      int8_t delta_x=(idx[help_x][help_y]>>3)-cursor_x,
              delta_y=(idx[help_x][help_y]&7 )-cursor_y;
      Direc direc=0;
      if(delta_x!=0||delta_y!=0){
        if(delta_x>0){direc=ARROW_RIGHT;}
        else if(delta_x<0){direc=ARROW_LEFT;}
        else if(delta_y>0){direc=ARROW_DOWN;}
        else if(delta_y<0){direc=ARROW_UP;}
        draw_arrow(cursor_x,cursor_y,~(color[help_x][help_y].val),direc);
      }else{
        draw_tick(cursor_x,cursor_y,0xffffff);
      }
      //printf("%d,%d\n",idx[help_x][help_y]>>3,idx[help_x][help_y]&7);
    }
    int key=read_key();
    if(key&0x8000){
      switch(stat){
        case GAME_PLAYING:
          draw_grid(cursor_x,cursor_y);
          switch(key^0x8000){
            case _KEY_SPACE:
              if(color[cursor_x][cursor_y].alpha==1)break;
              draw_circle(cursor_locat,w/40,0xffffff,0x00000000);
              if(choosen_idx==0){choosen_idx=1;choosen[0]=cursor_x;choosen[1]=cursor_y;}
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
            case _KEY_H:
              print_flag=!print_flag;
              break;
            default:
              break;
          }
        break;
        case GAME_WIN:
          if(key==(_KEY_R|0x8000)){
            init();
          }
          break;
        case GAME_PAUSE:
        default:
          printf("\nStat not handle!\n");
          return 0;
      }
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


void init_screen() {
  _DEV_VIDEO_INFO_t info = {0};
  _io_read(_DEV_VIDEO, _DEVREG_VIDEO_INFO, &info, sizeof(info));
  w = info.width;
  h = info.height;
}



void init(void){
  stat=GAME_PLAYING;
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
      idx[x][y]=(x<<3)+y;
    }
  }
  color[0][0].alpha=1;//Use alpha=1
  color[GRID_NUM-1][GRID_NUM-1].alpha=1;//to fix
  color[0][GRID_NUM-1].alpha=1;//specific
  color[GRID_NUM-1][0].alpha=1;//grids
  for(int i=0;i<50;++i){
      //swap times
      int j,k;
      do{
        j=rand()%(GRID_NUM*GRID_NUM);
      }while(color[j/GRID_NUM][j%GRID_NUM].alpha==1);
      do{
        k=rand()%(GRID_NUM*GRID_NUM);
      }while(color[k/GRID_NUM][k%GRID_NUM].alpha==1);
      {pixel temp=color[j/GRID_NUM][j%GRID_NUM];
      color[j/GRID_NUM][j%GRID_NUM]=color[k/GRID_NUM][k%GRID_NUM];
      color[k/GRID_NUM][k%GRID_NUM]=temp;}
      {uint8_t temp=idx[j/GRID_NUM][j%GRID_NUM];
      idx[j/GRID_NUM][j%GRID_NUM]=idx[k/GRID_NUM][k%GRID_NUM];
      idx[k/GRID_NUM][k%GRID_NUM]=temp;}
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

void swap_pixel(void){
  {pixel temp=color[choosen[0]][choosen[1]];
  color[choosen[0]][choosen[1]]=color[cursor_x][cursor_y];
  color[cursor_x][cursor_y]=temp;}
  {uint8_t temp=idx[choosen[0]][choosen[1]];
  idx[choosen[0]][choosen[1]]=idx[cursor_x][cursor_y];
  idx[cursor_x][cursor_y]=temp;}
  draw_grid(choosen[0],choosen[1]);
  draw_grid(cursor_x,cursor_y);
  int i;
  for(i=0;i<(1<<6);++i){
      if(i>=(1<<6))break;
      if(idx[0][i]!=i)break;
  }
  if(i==(GRID_NUM*GRID_NUM)){
    stat=GAME_WIN;
    *(int*)0=0;
    draw_str("You Win!\npress r to restart",0,0,2,0x3fff00);
  }
}
