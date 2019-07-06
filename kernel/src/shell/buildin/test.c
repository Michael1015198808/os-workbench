#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h> 
int test(void *args[]){
    open("./tags",O_RDWR);
    return 0;
}
