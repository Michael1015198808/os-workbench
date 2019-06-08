#include <assert.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <inttypes.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <regex.h>
#include <setjmp.h>
#include <signal.h>
#include <stddef.h>
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
#define BLOCK_LEN (0x20-sizeof(off_t))

#include "header.h"

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
        off_t off=sizeof(tab);
        write(fd,&off,sizeof(off_t));//BLOCK list's head
        write(fd,&off,sizeof(off_t));//BLOCK list's tail
        write(fd,zeros,HEADER_LEN-sizeof(off_t)*2);
        write(fd,zeros,sizeof(tab));
        write(fd,zeros,sizeof(string));//BLOCK list's first node(to simplify code)
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
static void string_puts(string str,int fd){
    while(str.next!=0){
        printf("%.*s",(int)BLOCK_LEN,str.info);
        read_db(fd,str.next,&str,sizeof(str));
    }
    puts(str.info);
}


#define SET_VALUE (1<<0)
#define SET_KEY   (1<<1)
#define set_value(...) 

void add_free_list(int fd,off_t cur){
    off_t tail_old;
    lseek(fd,sizeof(off_t),SEEK_SET);
    read(fd,&tail_old,sizeof(off_t));
    write_db(fd, tail_old+BLOCK_LEN, &cur, sizeof(off_t));
    off_t prev=cur;
    while(cur!=0){
        prev=cur;
        read_db(fd,cur+BLOCK_LEN,&cur,sizeof(off_t));
    }
    lseek(fd,sizeof(off_t),SEEK_SET);
    write(fd,&prev,sizeof(off_t));
}
off_t alloc_str(const char* src,int fd){
    lseek(fd,0,SEEK_SET);
    off_t list[2],ret;
    read(fd,list,sizeof(off_t)*2);
    int flag=1;//flag of left blocks
    if(list[0]!=list[1]){
        //If there are blocks left, starts from there
        ret=list[0];
        lseek(fd,ret+HEADER_LEN,SEEK_SET);
    }else{
        //Otherwise, starts from end
        ret=lseek(fd,0,SEEK_END)-HEADER_LEN;
        flag=0;
    }
    off_t prev=ret,cur=ret;
    int64_t len=strlen(src);
    while(len>0){
        lseek(fd,cur+HEADER_LEN,SEEK_SET);
        if(len<BLOCK_LEN){
            write(fd,src,len);
            write(fd,zeros,BLOCK_LEN-len);
        }else{
            write(fd,src,BLOCK_LEN);
        }
        src+=BLOCK_LEN;
        len-=BLOCK_LEN;
        if(flag){
            read(fd,&cur,sizeof(off_t));
            if(cur==list[1]){
                cur=lseek(fd,0,SEEK_END)-HEADER_LEN;
                flag=0;
            }
        }else{
            cur+=sizeof(string);
        }
        write_db(fd, prev+BLOCK_LEN,
                len>0? &cur:zeros,
                sizeof(off_t));
        prev=cur;
    }
    lseek(fd,0,SEEK_SET);
    if(flag){
        write(fd,&cur,sizeof(off_t));
    }else{
        write(fd,&list[1],sizeof(off_t));
    }
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
            off_t backup_val=cur_tab.value;
            cur_tab.value=alloc_str(value,db->fd);
            cur_tab.value_len=strlen(value);
            write_db(db->fd,cur_off,&cur_tab,sizeof(tab));
            add_free_list(db->fd,backup_val);
            return 0;
        }
    }
    cur_tab.value=alloc_str(value,db->fd);
    cur_tab.value_len=strlen(value);
    cur_tab.key=alloc_str(key,db->fd);
    cur_tab.key_len=strlen(key);
    cur_tab.next=lseek(db->fd,0,SEEK_END)-HEADER_LEN;
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
void kvdb_traverse(kvdb_t *db){
    tab cur_tab={.next=0};
    while(read_db(db->fd,cur_tab.next,&cur_tab,sizeof(tab)),
            cur_tab.next!=0){
        string key_str,val_str;
        read_db(db->fd,cur_tab.key,&key_str,sizeof(string));
        printf("Key:  ");
        string_puts(key_str,db->fd);
        read_db(db->fd,cur_tab.value,&val_str,sizeof(string));
        printf("Val:  ");
        string_puts(val_str,db->fd);
    }
}
