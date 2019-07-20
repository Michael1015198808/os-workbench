#include <stdint.h>
#include <stdio.h>

#ifdef READABLE
const char* const FMT="%02x ";
#else
const char* const FMT="%c";
#endif

#define hex(A,B,C,D) \
  ( ((D)<<6)+ \
    ((C)<<4)+ \
    ((B)<<2)+ \
    ((A)<<0) )

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
    {0x000,0x00ffffff},
    {0x004,0x00000000},

/* inodes */
    /* / */
    {0x040,0x00000001},// refcnt
    {0x044,0x00000480},// info
    {0x048,0x00000008},// size
    {0x04c,0x00000000},// type

    /* /test */
    {0x050,0x00000001},// refcnt
    {0x054,0x00000580},// info
    {0x058,0x00000000},// size
    {0x05c,0x00000000},// type

    /* /txt */
    {0x060,0x00000001},// refcnt
    {0x064,0x00000680},// info
    {0x068,0x00000005},// size
    {0x06c,0x00000001},// type

    /* /mnt */
    {0x070,0x00000001},// refcnt
    {0x074,0x00000780},// info
    {0x078,0x00000000},// size
    {0x07c,0x00000000},// type

    /* /dev */
    {0x080,0x00000001},// refcnt
    {0x084,0x00000880},// info
    {0x088,0x00000000},// size
    {0x08c,0x00000000},// type

    /* /proc */
    {0x090,0x00000001},// refcnt
    {0x094,0x00000980},// info
    {0x098,0x00000000},// size
    {0x09c,0x00000000},// type

    /* /home */
    {0x0a0,0x00000001},// refcnt
    {0x0a4,0x00000a80},// info
    {0x0a8,0x00000000},// size
    {0x0ac,0x00000000},// type

    /* /michael */
    {0x0b0,0x00000001},// refcnt
    {0x0b4,0x00000b80},// info
    {0x0b8,0x00000000},// size
    {0x0bc,0x00000000},// type

/* information */
    /* / */
    {0x400,0x00000000},// "/"'s inode
    {0x404,       '/'},// "/"'s name
    {0x480,0x00000000},// "/"'s info: /
    {0x484,0x00000500},// "/"'s info: test
    {0x488,0x00000600},// "/"'s info: txt
    {0x48c,0x00000700},// "/"'s info: mnt
    {0x490,0x00000800},// "/"'s info: dev
    {0x494,0x00000900},// "/"'s info: proc
    {0x498,0x00000a00},// "/"'s info: home

    /* /test */
    {0x500,0x00000001},// "/test"'s inode
    {0x504,0x74736574},// "/test"'s name
    {0x580,0x00000000},// "/test"'s info
    /* /txt */
    {0x600,0x00000002},// "/txt"'s inode
    {0x604,0x00747874},// "/txt"'s name
    {0x680,0x34333231},// "/txt"'s info
    {0x684,0x0000000a},// "/txt"'s info
    /* /mnt */
    {0x700,0x00000003},// "/mnt"'s inode
    {0x704,0x00746e6d},// "/mnt"'s name
    {0x780,0x00000000},// "/mnt"'s info
    /* /dev */
    {0x800,0x00000004},// "/dev"'s inode
    {0x804,0x00766564},// "/dev"'s name
    {0x880,0x00000000},// "/dev"'s info
    /* /proc */
    {0x900,0x00000005},// "/proc"'s inode
    {0x904,0x636f7270},// "/proc"'s name
    {0x980,0x00000000},// "/proc"'s info
    /* /home */
    {0xa00,0x00000006},// "/home"'s inode
    {0xa04,0x656d6f68},// "/home"'s name
    {0xa80,0x00000000},// "/home"'s info
    {0xa84,0x00000b00},// "/home"'s info
    // /home/
    /* michael */
    {0xb00,0x00000007},// "michael"'s inode
    {0xb04,0x6863696d},// "michael"'s name
    {0xb08,0x006c6561},// "michael"'s name
    {0xb80,0x00000006},// "michael"'s info
};
int main(){
    for(int i=1;i<sizeof(pairs)/sizeof(pairs[0]);++i){
        if(pairs[i].off<=pairs[i-1].off){
            fprintf(stderr,"%d\n",i);
            return 1;
        }
    }
    for(uint32_t i=0,j=0;i<0x10000;i+=4){
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
