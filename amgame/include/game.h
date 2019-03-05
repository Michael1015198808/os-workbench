#include <am.h>
#include <amdev.h>
#include <klib.h>

#define SIDE 16

//shape.c
void draw_circle(int x,int y,int r,uint32_t fg_color,uint32_t bg_color);
void mono_rect(int x, int y, int w, int h, uint32_t color);

//draw.c

//font.c
void draw_str(char* s,int x,int y,size_t size,int color);
/*static inline void puts(const char *s) {
  for (; *s; s++) _putc(*s);
}*/
