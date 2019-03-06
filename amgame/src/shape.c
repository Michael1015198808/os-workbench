#include <game.h>
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
  extern pixel color[GRID_NUM][GRID_NUM];
  mono_rect((x+MARGIN) * SIDE*3, (y+MARGIN) * SIDE*3, SIDE*3, SIDE*3, color[x][y].val);
  if(color[x][y].alpha==1){
      draw_cross(SIDE/2+coor_to_pix(x,y)+SIDE/2,SIDE,0xffffff,0x00000000);
  }
extern int choosen_idx,choosen[2];
  if(choosen_idx==1&&x==choosen[0]&&y==choosen[1]){
    draw_circle(coor_to_pix(x,y),SIDE,0xffffff,0x00000000);
  }
}
void draw_arrow(int x,int y,int color,Direc direc){
  int i;
  printf("%d\n",direc);
  if(direc&2){
    for(i=0;i<4;++i){
      mono_rect((x+MARGIN) * SIDE*3+(i+1)*SIDE/2, (y+MARGIN) * SIDE*3+SIDE*3/2, SIDE/2, SIDE/2, color);
    }
    for(i=0;i<2;++i){
      mono_rect((x+MARGIN) * SIDE*3+(i+3)*SIDE/2, (y+MARGIN) * SIDE*3+((3-i)*SIDE)/2, SIDE/2, SIDE/2, color);
      mono_rect((x+MARGIN) * SIDE*3+(i+3)*SIDE/2, (y+MARGIN) * SIDE*3+((3+i)*SIDE)/2, SIDE/2, SIDE/2, color);
    }
  }else{
    *(int*)0=0;
  }
}
