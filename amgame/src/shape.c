#include <game.h>
extern pixel color[GRID_NUM][GRID_NUM];
extern int cursor_x,cursor_y;
extern int choosen_idx,choosen[2];

void draw_circle(int x,int y,int r,uint32_t fg_color,uint32_t bg_color){
    x+=r/2;y+=r/2;
    int cur_x=r,cur_y=0;
    for(;cur_x>0;--cur_x){
        while((cur_x*cur_x)+(cur_y*cur_y)<r*r){
          mono_rect(x+cur_x,y+cur_y,2,2,fg_color);
          mono_rect(x-cur_x,y+cur_y,2,2,fg_color);
          mono_rect(x+cur_x,y-cur_y,2,2,fg_color);
          mono_rect(x-cur_x,y-cur_y,2,2,fg_color);
          --cur_y;
        }
        ++cur_y;
    }
    mono_rect(x,y+r,1,1,fg_color);
    mono_rect(x,y-r,1,1,fg_color);
}
void draw_cross(int x,int y,int len,uint32_t fg_color,uint32_t bg_color){
    int i;
    for(i=0;i<len;++i){
        mono_rect(x+i,y+i,2,2,fg_color);
        mono_rect(x-i,y+i,2,2,fg_color);
        mono_rect(x+i,y-i,2,2,fg_color);
        mono_rect(x-i,y-i,2,2,fg_color);
    }
}
void mono_rect(int x, int y, int w, int h, uint32_t color) {
  uint32_t pixels[w * h];
  _DEV_VIDEO_FBCTL_t event = {
    .x = x, .y = y, .w = w, .h = h, .sync = 1,
    .pixels = pixels,
  };
  for (int i = 0; i < w * h; i++) {
    pixels[i] = color;
  }
  _io_write(_DEV_VIDEO, _DEVREG_VIDEO_FBCTL, &event, sizeof(event));
}
void draw_grid(int x,int y){
  mono_rect((x+MARGIN) * SIDE*3, (y+MARGIN) * SIDE*3, SIDE*3, SIDE*3, color[x][y].val);
  if(color[x][y].alpha==1){
      draw_cross(SIDE/2+coor_to_pix(x,y)+SIDE/2,SIDE,0xffffff,0x00000000);
  }
  if(choosen_idx==1&&x==choosen[0]&&y==choosen[1]){
    draw_circle(coor_to_pix(x,y),SIDE,0xffffff,0x00000000);
  }
}
void draw_arrow(int x,int y,int color,Direc direc){
  int i;
  /*char* s[]={"left","right","up","down"};
  printf("%s\n",s[direc]);*/
  if(direc&2){
      mono_rect((x+MARGIN) * SIDE*3+(SIDE*5)/4, (y+MARGIN) * SIDE*3+(SIDE*3)/4, SIDE/2, (SIDE*3)/2, color);
    for(i=1;i<4;++i){
      mono_rect((x+MARGIN) * SIDE*3+(6-i)*SIDE/4, (y+MARGIN) * SIDE*3+((direc&1?(10-i):(2+i))*SIDE)/4, SIDE/4, SIDE/4, color);
      mono_rect((x+MARGIN) * SIDE*3+(5+i)*SIDE/4, (y+MARGIN) * SIDE*3+((direc&1?(10-i):(2+i))*SIDE)/4, SIDE/4, SIDE/4, color);
    }
  }else{
      mono_rect((x+MARGIN) * SIDE*3+(SIDE*3)/4, (y+MARGIN) * SIDE*3+(SIDE*5)/4, (SIDE*3)/2, SIDE/2, color);
    for(i=1;i<4;++i){
      mono_rect((x+MARGIN) * SIDE*3+(direc&1?(10-i):(2+i))*SIDE/4, (y+MARGIN) * SIDE*3+((6-i)*SIDE)/4, SIDE/4, SIDE/4, color);
      mono_rect((x+MARGIN) * SIDE*3+(direc&1?(10-i):(2+i))*SIDE/4, (y+MARGIN) * SIDE*3+((5+i)*SIDE)/4, SIDE/4, SIDE/4, color);
    }
  }
}
void draw_cursor(int mode){
// 0 for wipe
// 1 for draw
  static uint32_t old_time=0,new_time,is_white=1;
  new_time=uptime();
  mono_rect(
    (cursor_x+MARGIN) * SIDE*3+SIDE,
    (cursor_y+MARGIN) * SIDE*3+SIDE,
    SIDE,
    SIDE,
    mode==0?color[cursor_x][cursor_y].val:
    (is_white?0xffffff:0x000000));
  if(new_time-old_time>100){
      old_time=new_time;
      is_white=!is_white;
  }
}
void draw_tick(int x,int y,int color){
  int i;
  for(i=0;i<3;++i){
    mono_rect(
      (x+MARGIN) * SIDE*3+((2+i)*SIDE)/4,
      (y+MARGIN) * SIDE*3+((8+i)*SIDE)/5,
      SIDE/4,
      SIDE/4, color);
  }
  for(i=0;i<7;++i){
    mono_rect(
      (x+MARGIN) * SIDE*3+((6+i)*SIDE)/5,
      (y+MARGIN) * SIDE*3+((9-i)*SIDE)/4,
      SIDE/4,
      SIDE/4, color);
  }
}
