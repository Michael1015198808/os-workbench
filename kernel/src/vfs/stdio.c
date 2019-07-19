// Realized fprintf which need the support of OS
#include <common.h>
#include <klib.h>
#include <vfs.h>

int fprintf(int fd,const char* fmt, ...){
    char buf[0x100];
    va_list ap;
    va_start(ap,fmt);
    int len=vsprintf(buf,fmt,ap);//vsprintf will call va_end(ap);
    return vfs->write(fd,buf,len);
}
