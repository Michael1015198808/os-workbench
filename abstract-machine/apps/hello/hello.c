#include <am.h>

void print(const char *s) {
  for (; *s; s ++) {
    _putc(*s);
  }
}
int main() {
  for (int i = 0; i < 10; i ++) {
    print("Hello World from a(n) " __ISA__ " program!\n");
  }
  return 0;
}
