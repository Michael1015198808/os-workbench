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

#define safe_do(Sentence) \
    { \
        if(Sentence) \
        return -1; \
    }

#define HEADER_LEN 0x100
//Reserved in case for further usage
#define BLOCK_LEN 0x20

typedef struct string{
    char info[BLOCK_LEN];
    off_t next;
}string;

typedef struct tab{
    off_t next,key,key_len,value,value_len;
}tab;

static void read_db(int fd,off_t off,off_t len,void *dst){
    lseek(fd,HEADER_LEN+off,SEEK_SET);
    read(fd,dst,len);
}

static int string_cmp(const char* key,string str,int fd){
    while(str.next!=-1){
        int ret;
        ret=strncmp(str.info,key,BLOCK_LEN);
        if(ret)return ret;
        read_db(fd,str.next,sizeof(string),&str);
    }
    return strncmp(str.info,key,BLOCK_LEN);
}
static void string_cpy(char* dst,string str,int fd){
    while(str.next!=-1){
        strncpy(dst,str.info,BLOCK_LEN);
        dst+=BLOCK_LEN;
        read_db(fd,str.next,sizeof(string),&str);
    }
    strcpy(dst,str.info);
}

int kvdb_open(kvdb_t *db, const char *filename){
    db->fd=open(filename,O_RDWR|O_CREAT,0777);
    return db->fd>=0;
}

int kvdb_close(kvdb_t *db){
    return close(db->fd);
}
static off_t alloc_block(void){
    return 0;
}
static void set_value(int fd,tab t,const char* value){
    off_t len=strlen(value);
    string val_str;
    if(t.value!=-1){
        read_db(fd,t.value,sizeof(string),&val_str);
    }else{
        //Alloc a string
    }
    while(len>0){
        strncpy(val_str.info,value,BLOCK_LEN);
        value+=BLOCK_LEN;
        len-=BLOCK_LEN;
        if(val_str.next==-1){
            //Alloc a string
        }else{
            read_db(fd, val_str.next, sizeof(string), &val_str);
        }
    }
    return;
}

static inline int _kvdb_put(kvdb_t *db, const char *key, const char *value){
    tab cur_tab={.next=0};
    do{
        read_db(db->fd,cur_tab.next,sizeof(tab),&cur_tab);
        string key_str;
        read_db(db->fd,cur_tab.key,sizeof(string),&key_str);
        if(!string_cmp(key,key_str,db->fd)){
            set_value(db->fd,cur_tab,value);
            return 0;
        }
    }while(cur_tab.next!=-1);
    set_value(db->fd,cur_tab,value);
    return 0;
}
int kvdb_put(kvdb_t *db, const char *key, const char *value){
    flock(db->fd,LOCK_EX);
    return _kvdb_put(db,key,value);
    flock(db->fd,LOCK_UN);
}

static inline char *_kvdb_get(kvdb_t *db, const char *key){
    tab cur_tab={.next=0};
    do{
        read_db(db->fd,cur_tab.next,sizeof(tab),&cur_tab);
        string key_str;
        read_db(db->fd,cur_tab.key,sizeof(string),&key_str);
        if(!string_cmp(key,key_str,db->fd)){
            string val_str;
            read_db(db->fd,cur_tab.value,sizeof(string),&val_str);
            char *ret=malloc(cur_tab.value_len+1);
            string_cpy(ret,val_str,db->fd);
            return ret;
        }
    }while(cur_tab.next!=-1);
    return NULL;
}
char *kvdb_get(kvdb_t *db, const char *key){
    flock(db->fd,LOCK_SH);
    char *ret=_kvdb_get(db,key);
    flock(db->fd,LOCK_UN);
    return ret;
}
