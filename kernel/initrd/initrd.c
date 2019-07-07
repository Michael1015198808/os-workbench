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
    {0x000,0x00000210},
    {0x100,0x00000000},//"/"
    {0x104,0x00000110},//"/"'s info's offset
    {0x108,0x02100150},//"/"'s name's offset
    {0x10c,CHECK_CODE},
    {0x110,0x00000190},//"/"'s info
    {0x150,       '/'},//"/"'s name'
    {0x190,0x00000000},//"/test"
    {0x194,0x000001a0},//"/test"'s info's offset
    {0x198,0x000001e0},//"/test"'s name's offset
    {0x19c,CHECK_CODE},
    {0x1e0,0x74736574},//"/test"'s name 
};
int main(){
    for(uint32_t i=0,j=0;i<0x300;){
#ifdef READABLE
        if(!(i&0xf)){printf("\n %03x: ",i);}
#endif
        if(i==pairs[j].off){
            write_xxd(pairs[j].info);
            i+=4;
            ++j;
        }else{
            printf(FMT,0x00);
            i+=1;
        }
    }
    return 0;
}
