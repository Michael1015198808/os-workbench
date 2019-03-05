#include <game.h>
void draw_circle(int x,int y,int r,uint32_t fg_color,uint32_t bg_color){
    
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
