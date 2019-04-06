#include <unistd.h>
#include <klib.h>
int main(int argc, char *argv[]) {
  if(fork()==0){
    printf("Hello\n");
  }
  return 0;
}
