#include <common.h>
#include <klib.h>
static inline void fill(uint8_t *p,int a,int b,int len){
    for(int i=0;i<len;++i){
        p[i]=(a+=b);
    }
}
static inline void check(uint8_t *p,int a,int b,long long len){
    for(int i=0;i<len;++i){
        Assert(p[i]==(uint8_t)(a+=b));
    }
}
void memory_test(){
    int a[50],b[50],cpu_id=_cpu();
    long long len[50];
    void *p[50];
    for(int i=0;i<50;++i){
        len[i]=(1<<((i&15)+5))+rand();
        a[i]=rand()+cpu_id;
        b[i]=rand()+cpu_id;
        p[i]=pmm->alloc(len[i]);
        fill(p[i],a[i],b[i],len[i]);
    }
    for(int j=0;j<1000;++j){
        int i=rand()%50;
        check(p[i],a[i],b[i],len[i]);
        pmm->free(p[i]);
        int shift=13+(rand()&7);
        len[i]=rand()+((1<<shift)-1);
        if(i&1){
            len[i]+=1<<12;
        }
        a[i]=rand()+cpu_id;
        b[i]=rand()+cpu_id;
        p[i]=pmm->alloc(len[i]);
        fill(p[i],a[i],b[i],len[i]);
    }
    printf("[cpu%d] finish memory test.\n",_cpu());
    while(1);
}
