#include <stdint.h>
#include <stdio.h>

#ifdef READABLE
const char* const FMT="%02x ";
#else
const char* const FMT="%c";
#endif

#define CHECK_CODE 0x0a736c79
void write_xxd(uint32_t info){
    uint8_t *c=(void*)&info;
    for(int i=0;i<4;++i){
        printf(FMT,c[i]);
    }
}
/*
0123
4567
89ab
cdef
*/
struct pair{
    uint32_t off,info;
}pairs[]={
    {0x000,0x00000000},
    {0x004,0x00000000},

/* inodes */
    /* / */
    {0x040,0x00000001},// refcnt
    {0x044,0x00000480},// info
    {0x048,0x00000008},// size

    /* /test */
    {0x050,0x00000001},// refcnt
    {0x054,0x00000580},// info
    {0x058,0x00000000},// size

    /* /txt */
    {0x060,0x00000001},// refcnt
    {0x064,0x00000680},// info
    {0x068,0x00000004},// size

/* information */
    /* / */
    {0x400,0x00000000},// "/"'s inode
    {0x404,       '/'},// "/"'s name
    {0x480,0x00000200},// "/"'s info

    /* /test */
    {0x500,0x00000001},// "/"'s inode
    {0x504,0x74736574},// "/test"'s name
    {0x580,0x00000000},// "/test"'s info
    /* /txt */
    {0x600,0x00000002},// "/txt"'s inode
    {0x680,0x00747874},// "/txt"'s name
    {0x680,0x34333231},// "/txt"'s info
};
int main(){
    for(uint32_t i=0;i<0x40;++i){
    }
    for(uint32_t i=0x40,j=0;i<0x10000;i+=4){
#ifdef READABLE
        if(!(i&0xf)){printf("\n %03x: ",i);}
#endif
        if(i==pairs[j].off){
            write_xxd(pairs[j].info);
            ++j;
        }else{
            write_xxd(0);
        }
    }
    return 0;
}
