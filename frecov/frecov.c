#include <assert.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <inttypes.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <regex.h>
#include <setjmp.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>
int main(int argc, char *argv[]) {
    int fd = open("./test_file", O_RDWR);
    struct stat st;
    fstat(fd, &st);
    uint8_t *byte_ptr = mmap(NULL, st.st_size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);

    // read a byte from the file
    uint8_t byte = byte_ptr[12];
    for(int i=0;i<15;++i){
        putchar(byte_ptr[i]);
    }

    close(fd);
    return 0;
}
