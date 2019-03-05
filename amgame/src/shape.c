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
    }
    mono_rect(x,y+r,1,1,fg_color);
    mono_rect(x,y-r,1,1,fg_color);
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
