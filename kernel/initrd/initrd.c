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
    {0x044,0x00000180},// info

    /* /test */
    {0x048,0x00000001},// refcnt
    {0x04c,0x00000280},// info

    /* /txt */
    {0x050,0x00000001},// refcnt
    {0x054,0x00000380},// info

/* information */
    /* / */
    {0x100,0x00000000},// "/"'s inode
    {0x104,       '/'},// "/"'s name
    {0x180,0x00000200},// "/"'s info

    /* /test */
    {0x200,0x00000001},// "/"'s inode
    {0x204,0x74736574},// "/test"'s name
    {0x280,0x00000000},// "/test"'s info
    /* /txt */
    {0x300,0x00000002},// "/txt"'s inode
    {0x380,0x00747874},// "/txt"'s name
    {0x380,0x34333231},// "/txt"'s info
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
