#include <assert.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <inttypes.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <regex.h>
#include <setjmp.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/file.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

#include "kvdb.h"

#define log(pat, ...) \
    printf("%d" pat, __LINE__##__VA_ARGS__)

#define safe_do(Sentence) \
    { \
        if(Sentence) \
        return -1; \
    }

#define HEADER_LEN 0x100
//Reserved in case for further usage
#define BLOCK_LEN 0x20

//All offset doesn't consider header
typedef struct string{
    char info[BLOCK_LEN];
    off_t next;
}string;

typedef struct tab{
    off_t next,key,key_len,value,value_len;
}tab;

static struct{
    union{
        //Make sure zeros is as longer as the longest constant considered
        uint8_t info1[HEADER_LEN];
        uint8_t info2[sizeof(tab)];
    };
    uint8_t margin[0x10];
}padding={};
void *zeros=&padding;

//[read|write]_db considers the offset caused by header
//manually add HEADER_LEN only when you use lseek/write instead
//If possible, use [read|write]_db to decrease bug.
static int read_db(int fd,off_t off,void *dst,off_t len){
    lseek(fd,HEADER_LEN+off,SEEK_SET);
    return read(fd,dst,len);
}

static int write_db(int fd,off_t off,void *src,off_t len){
    lseek(fd,HEADER_LEN+off,SEEK_SET);
    return write(fd,src,len);
}
//To prevent write in kvdb_ s
static inline void init_db(int fd){
        lseek(fd,0,SEEK_SET);
        write(fd,zeros,HEADER_LEN);
        write(fd,zeros,sizeof(tab));
}
//Read/Write reserverd area isn't supported by these API

static int string_cmp(const char* key,string str,int fd){
    while(str.next!=0){
        int ret=strncmp(key,str.info,BLOCK_LEN);
        if(ret)return ret;
        key+=BLOCK_LEN;
        read_db(fd,str.next,&str,sizeof(string));
    }
    return strncmp(key,str.info,BLOCK_LEN);
}
static void string_cpy(char* dst,string str,int fd){
    while(str.next!=0){
        strncpy(dst,str.info,BLOCK_LEN);
        dst+=BLOCK_LEN;
        read_db(fd,str.next,&str,sizeof(string));
    }
    strcpy(dst,str.info);
}


#define SET_VALUE (1<<0)
#define SET_KEY   (1<<1)
#define set_value(...) 
off_t alloc_str(const char* src,int fd){
    off_t ret=lseek(fd,0,SEEK_END)-HEADER_LEN,cur=ret;
    int len=strlen(src);
    while(len>BLOCK_LEN){
        write(fd,src,BLOCK_LEN);
        src+=BLOCK_LEN;
        len-=BLOCK_LEN;
        cur+=BLOCK_LEN+sizeof(off_t);
        write(fd,&cur,sizeof(off_t));
    }
    write(fd,src,len);
    write(fd,zeros,
/*align to 4*/((4-len)&3)
/*set next=0*/+sizeof(off_t));
    return ret;
}

int kvdb_open(kvdb_t *db, const char *filename){
    db->fd=open(filename,O_RDWR|O_CREAT,0777);
    if(db->fd<0)return db->fd;
    flock(db->fd,LOCK_EX);
    if(lseek(db->fd,0,SEEK_END)<HEADER_LEN){
        init_db(db->fd);
    }
    flock(db->fd,LOCK_UN);
    return 0;
}

int kvdb_close(kvdb_t *db){
    return close(db->fd);
}

static inline int _kvdb_put(kvdb_t *db, const char *key, const char *value){
    tab cur_tab={.next=0};
    off_t cur_off=0;(void)cur_off;
    while(  cur_off=cur_tab.next,
            read_db(db->fd,cur_tab.next,&cur_tab,sizeof(tab)),
            cur_tab.next!=0){
        string key_str;
        read_db(db->fd,cur_tab.key,&key_str,sizeof(string));
        if(!string_cmp(key,key_str,db->fd)){
            cur_tab.value=alloc_str(value,db->fd);
            cur_tab.value_len=strlen(value);
            write_db(db->fd,cur_off,&cur_tab,sizeof(tab));
            return 0;
        }
    }
    cur_tab.value=alloc_str(value,db->fd);
    cur_tab.value_len=strlen(value);
    cur_tab.key=alloc_str(key,db->fd);
    cur_tab.key_len=strlen(key);
    cur_tab.next=lseek(db->fd,0,SEEK_END)-BLOCK_LEN;
    write(db->fd,zeros,sizeof(tab));
    write_db(db->fd,cur_off,&cur_tab,sizeof(tab));
    return 0;
}
int kvdb_put(kvdb_t *db, const char *key, const char *value){
    flock(db->fd,LOCK_EX);
    int ret=_kvdb_put(db,key,value);
    flock(db->fd,LOCK_UN);
    return ret;
}

static inline char *_kvdb_get(kvdb_t *db, const char *key){
    tab cur_tab={.next=0};
    while(read_db(db->fd,cur_tab.next,&cur_tab,sizeof(tab)),
            cur_tab.next!=0){
        string key_str;
        read_db(db->fd,cur_tab.key,&key_str,sizeof(string));
        if(!string_cmp(key,key_str,db->fd)){
            string val_str;
            read_db(db->fd,cur_tab.value,&val_str,sizeof(string));
            char *ret=malloc(cur_tab.value_len+1);
            string_cpy(ret,val_str,db->fd);
            return ret;
        }
    }
    return NULL;
}
char *kvdb_get(kvdb_t *db, const char *key){
    flock(db->fd,LOCK_SH);
    char *ret=_kvdb_get(db,key);
    flock(db->fd,LOCK_UN);
    return ret;
}
