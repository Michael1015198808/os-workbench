#include "klib.h"

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

static uint8_t buffer[0x5000];
size_t strlen(const char *s) {
    int i=0;
    while(s[i]!='\0'){
        ++i;
    }
    return i;
}


char *strcpy(char* dst,const char* src) {
    size_t i=0;
    for(i=0;src[i]!='\0';++i){
        dst[i]=src[i];
    }
    dst[i]=src[i];
    return dst;
}

char* strncpy(char* dst, const char* src, size_t n) {
    size_t i=0;
    for(i=0;i<n&&src[i]!='\0';++i){
        dst[i]=src[i];
    }
    for(;i<n;++i){
        dst[i]='\0';
    }
    return dst;
}

char* strcat(char* dst, const char* src) {
    size_t dest_len=strlen(dst);
    size_t i;
    for(i=0;src[i]!='\0';++i){
        dst[dest_len+i]=src[i];
    }
    dst[dest_len+i]='\0';
    return dst;
}

int strcmp(const char* s1, const char* s2) {
    while(*s1==*s2&&*s1!='\0'){
        ++s1;
        ++s2;
    }
    return *s1-*s2;
}

int strncmp(const char* s1, const char* s2, size_t n) {
    if(n==0){return 0;}
    while(n>0&&*s1==*s2){
        ++s1;
        ++s2;
        --n;
    }
    return *s1-*s2;
}

void* memset(void* v,int c,size_t n) {
    size_t i;
    for(i=0;i<n;++i){
        *((char*)v+i)=c;
    }
    return v;
}

void* memcpy(void* out, const void* in, size_t n) {
    size_t i;
    for(i=0;i<n;++i){
        *((char*)out+i)=*((char*)in+i);
    }
    return out;
}

int memcmp(const void* s1, const void* s2, size_t n){
    if(n==0){return 0;}
    size_t i;
    for(i=0;i<n;++i){
      if(((char*)s2)[i]==((char*)s1)[i]){
          continue;
      }else{
          return ((char*)s2)[i]-((char*)s1)[i];
      }
    }
    return 0;
}

void *memmove(void *dest, const void *src, size_t n){
    pthread_mutex_t lk=PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&lk);
    if(n>sizeof(buffer)){
        printf("%s %d:want to move memory larger than memmove's buf(%x bytes)!\n",__FILE__,__LINE__,n);
    }
    memcpy(buffer,src,n);
    memcpy(dest,buffer,n);
    pthread_mutex_unlock(&lk);
    return dest;
}

//Return *s in s such that *s == c
//If no such *s exists, return NULL
char* strchr(const char *s, char c){
    while(*s){
        if(*s == c){
            return (char*)s;
        }else{
            ++s;
        }
    }
    return NULL;
}

#endif
