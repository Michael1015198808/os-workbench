#include <am.h>
#include <amdev.h>
#include <klib.h>

#define GRID_NUM 8
#define MARGIN 1
#define cursor_locat ((cursor_x+MARGIN) * SIDE*3+SIDE),((cursor_y+MARGIN) * SIDE*3+SIDE)
#define coor_to_pix(_x,_y) ((_x+MARGIN) * SIDE*3+SIDE),((_y+MARGIN) * SIDE*3+SIDE)
#define SIDE w/40
#define SHUFFLE_TIMES 2
union pixel{
  uint32_t val;
  struct{
    uint32_t b:8,g:8,r:8,alpha:8;
    //It seems alpha is useless
  };
};
typedef union pixel pixel;

int w,h;
enum{GAME_PLAYING,GAME_PAUSE,GAME_WIN}stat;
pixel color[GRID_NUM][GRID_NUM];
int choosen[2],choosen_idx;
uint8_t idx[GRID_NUM][GRID_NUM];
int cursor_x,cursor_y;
//shape.c
void draw_cross(int x,int y,int len,uint32_t fg_color,uint32_t bg_color);
void draw_circle(int x,int y,int r,uint32_t fg_color,uint32_t bg_color);
void draw_grid(int x,int y);
void mono_rect(int x, int y, int w, int h, uint32_t color);
typedef enum{ARROW_LEFT=0,ARROW_RIGHT=1,ARROW_UP=2,ARROW_DOWN=3} Direc;
void draw_arrow(int x,int y,int color,Direc);
void draw_tick(int x,int y,int color);
void draw_cursor(int);

//logic.c
void init(void);
void operate(int);
int next_key();

//draw.c
void init_screen();
void print_instr();

//font.c
void draw_str(char* s,int x,int y,size_t size,int color);
