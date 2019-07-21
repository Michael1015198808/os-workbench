#include <common.h>
#include <klib.h>

#ifndef NO_TEST
#define MAXN 50
#define MAXM 200
void memory_test_init(void){
    return;
}
static inline void fill(uint8_t *p,int a,int b,int len){
    for(int i=0;i<len;++i){
        p[i]=(a+=b);
    }
}
static inline void check(uint8_t *p,int a,int b,long long len){
    for(int i=0;i<len;++i){
        Assert(p[i]==(uint8_t)(a+=b),"Memory test failed!");
    }
}
void memory_test(void *dummy){
    int *a=pmm->alloc(MAXN*sizeof(int)),
        *b=pmm->alloc(MAXN*sizeof(int));
    int cpu_id=_cpu();
    long long len[MAXN];
    void *p[MAXN];
    for(int i=0;i<MAXN;++i){
        len[i]=(1<<((i&15)+5))+rand();
        a[i]=rand()+cpu_id;
        b[i]=rand()+cpu_id;
        p[i]=pmm->alloc(len[i]);
        fill(p[i],a[i],b[i],len[i]);
    }
    for(int j=0;j<MAXM;++j){
        int i=rand()%MAXN;
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
    for(int i=0;i<MAXN;++i)
        pmm->free(p[i]);
    printf("[cpu%d] finish memory test.\n",_cpu());
}

#endif
