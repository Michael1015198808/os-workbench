int main(int argc, char *argv[]) {
  if(fork()==0){
    printf("Hello\n");
  }
  return 0;
}
