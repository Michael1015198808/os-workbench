#include <stdio.h>
#include <assert.h>
#include <unistd.h>
#include <sys/types.h>

int main(int argc, char *argv[]) {
  printf("Hello, World!\n");
  int i;
  for (i = 0; i < argc; i++) {
    assert(argv[i]); // always true
    printf("argv[%d] = %s\n", i, argv[i]);
  }
  printf("%d\n",getpid());
  getchar();//Give me time to find it in /proc
  assert(!argv[argc]); // always true
  return 0;
}
