#include <game.h>
extern int print_flag;
void init_screen() {
  _DEV_VIDEO_INFO_t info = {0};
  _io_read(_DEV_VIDEO, _DEVREG_VIDEO_INFO, &info, sizeof(info));
  w = info.width;
  h = info.height;
}
void print_instr(){
    //print cursor or assistant arrow
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
    }
}
