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

#define offset_of(member,struct) ((uintptr_t)&(((struct*)0)->member))
#define pstruct struct __attribute__((packed))
#define my_cmp(pat, ptr) strncmp(pat,ptr,sizeof(pat))
typedef union bmp{
    uint8_t info[0];
    
    pstruct{
        pstruct {
            uint16_t type;
            uint32_t size;
            uint16_t unused[2];
            uint32_t offset;
        }bfh;
        //Bitmap file header
        pstruct{
            uint32_t size;
            uint16_t width,height,planes,bit_per_pixel;
        }dibh;
        //DIB header
    };
}bmp_t;
_Static_assert(offset_of(dibh,bmp_t)==14,"Offset of DIB header is wrong!");
int main(int argc, char *argv[]) {
    printf("%d\n",(int)offset_of(dibh,bmp_t));
    return 0;
    int fd = open("./fs.img", O_RDONLY);
    struct stat st;
    fstat(fd, &st);
    uint8_t *fs = mmap(NULL, st.st_size, PROT_READ , MAP_SHARED, fd, 0);

    // read a byte from the file
    for(uint64_t i=0;i<st.st_size;i+=0x100){
        if(my_cmp("BM",fs+i));
    }

    close(fd);
    return 0;
}
