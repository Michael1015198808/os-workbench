#include <am.h>
#include <amdev.h>
#include <klib.h>

#define SIDE 16

void mono_rect(int x, int y, int w, int h, uint32_t color);
void draw_str(char* s,int x,int y,size_t size,int color);
/*static inline void puts(const char *s) {
  for (; *s; s++) _putc(*s);
}*/
