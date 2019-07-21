#include <common.h>

void* dequeue(pool* p){
    pthread_mutex_lock(&p->lk);

    void* ret=NULL;
    if(p->head!=p->tail){
        ret=p->mem[p->head];
        ++p->head;
        if(p->head==POOL_LEN)p->head=0;
    }

    pthread_mutex_unlock(&p->lk);
    return ret;
}

void enqueue(pool* p,void* mem){
    pthread_mutex_lock(&p->lk);

    while((p->tail+1)%POOL_LEN==p->head){
        pthread_mutex_unlock(&p->lk);
        _yield();
        pthread_mutex_lock(&p->lk);
    }
    p->mem[p->tail]=mem;
    ++p->tail;
    if(p->tail==POOL_LEN)p->tail=0;

    pthread_mutex_unlock(&p->lk);
}


