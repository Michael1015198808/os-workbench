/* 
 * Author: Michael Yan
 * STUID: 171240518
 * Features:
 *      Concurrent read/write
 *      Proper RW control(Between processes and between threads)
 *      recycle string of value being updated.
 *      store strings in blocks like linked list
 *      store key-value pairs like linked list
 * Fancy designs:
 *      declare a zeros array to write 00...00 faster.
 *      Use union padding to make sure zeros is long enough.
 *      macro safe_call
 *      When a thread want to write, prevent new threads to read, until this thread finishs writing, so even there are endless reading, write can still success.
 *        algorithm:
 *          Before a writer gain the writer-lock, it add wr_cnt atomically. So that wr_cnt is always equals to the number of writers waiting for lock.
 *          Before a reader gain the reader-lock, it check whether wr_cnt>0(If there are writers need lock). If so, it add rd_cnt(not atommatically for speeding up). If rd_cnt is large enough
 *          (writer has waited a long time), readers park by set rd_block until a writer finishs writing and reset rd_block.
 *        proof of rd_cnt without lock:(Prove by the value of wr_acq when a writer set rd_cnt to 0)
 *          I)   If wr_acq>0, no reader modify rd_cnt. The writer can modify the value atommically.
 *              (All readers set rd_block to 1 in this case. That's why writer should set rd_cnt to 0 before reset rd_block)
 *          II)  If wr_acq=0, all possible operation are setting rd_cnt=0. Though no atomic, it's still okay.
 * 
 */
#include <assert.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <inttypes.h>
#include <pthread.h>
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

#define HEADER_LEN 0x100
//Reserved in case for further usage
#define BLOCK_LEN (0x20-sizeof(uint32_t))


#define SAFE
#if defined(DEBUG)&&defined(SAFE)
_Static_assert(0,"DEBUG and SAFE can't be both defined!");
#endif
//SAFE mode default

#ifdef DEBUG
    #define safe_call(call,cond) \
        (may_bug(), \
        call)
void may_bug(void){
    if(rand()==0){
        asm volatile("int $3");//For debug usage
        exit(1);
    }
}

#else
__thread int ret;
#define HANDLER \
    exit(1)

#define safe_call(call,cond) \
            ( \
                ret=call, \
                ret cond?   \
                ret: \
                ( \
                    fprintf(stderr, \
                    "error in "__FILE__ ":%d(%s)" \
                    #call " returns %d\n", __LINE__, __func__, ret), \
/*exit returns void*/HANDLER, \
/*so an int needed.*/0 \
                ) \
            )
#endif

//All offset doesn't consider header
typedef struct string{
    char info[BLOCK_LEN];
    uint32_t next;
}string;

typedef struct tab{
    uint32_t next,key,key_len,value,value_len;
}tab;

#include "header.h"

const static struct{
    union{
        //Make sure zeros is as longer as the longest constant considered
        uint8_t info1[HEADER_LEN];
        uint8_t info2[sizeof(tab)];
    };
    uint8_t margin[0x10];
}padding={};
const void const*zeros=&padding;
static uint8_t useless_buf[sizeof(padding)];

//[read|write]_db considers the offset caused by header
//manually add HEADER_LEN only when you use lseek/write instead
//If possible, use [read|write]_db to decrease bug.
static int read_db(int fd,uint32_t off,void *dst,uint32_t len){
    return pread(fd,dst,len,HEADER_LEN+off);
}

static int write_db(int fd,uint32_t off,const void *src,uint32_t len){
    return pwrite(fd,src,len,HEADER_LEN+off);
}
//To prevent write in kvdb_ s
static inline void init_db(int fd){
        lseek(fd,0,SEEK_SET);
        uint32_t off=sizeof(tab);
        uint32_t size=HEADER_LEN+sizeof(tab)+sizeof(string);
        uint32_t cnt=0;
        cnt+=pwrite(fd,&off,sizeof(uint32_t),0);//free list's head
        cnt+=pwrite(fd,&off,sizeof(uint32_t),cnt);//free list's tail
        cnt+=pwrite(fd,&size,sizeof(uint32_t),cnt);//free list's tail
        cnt+=pwrite(fd,zeros,HEADER_LEN-sizeof(uint32_t)*3,cnt);
        cnt+=pwrite(fd,zeros,sizeof(tab),cnt);
        cnt+=pwrite(fd,zeros,sizeof(string),cnt);//free list's first node(to simplify code)
        if(cnt!=size){
            asm volatile("int $0x3");
        }
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

void add_free_list(int fd,uint32_t cur){
    uint32_t tail_old;
    pread(fd,&tail_old,sizeof(uint32_t),sizeof(uint32_t));
    write_db(fd, tail_old+BLOCK_LEN, &cur, sizeof(uint32_t));
    uint32_t prev=cur;
    while(cur!=0){
        prev=cur;
        read_db(fd,cur+BLOCK_LEN,&cur,sizeof(uint32_t));
    }
    pwrite(fd,&prev,sizeof(uint32_t),sizeof(uint32_t));
}
static uint32_t get_end(int fd,uint32_t append){
    uint32_t ret,new_end;
    pread(fd,&ret,sizeof(uint32_t),offsetof(header,free_list.size));
    new_end=ret+append;
    pwrite(fd,&new_end,sizeof(uint32_t),offsetof(header,free_list.size));
    return ret;
}
uint32_t alloc_str(const char* src,int fd){
    uint32_t list[2],ret;
    pread(fd,list,sizeof(uint32_t)*2,0);
    int flag=1;//flag of left blocks
    if( ( flag=(list[0]!=list[1]) )  ){
        //If there are blocks left, starts from there
        ret=list[0];
        lseek(fd,ret+HEADER_LEN,SEEK_SET);
    }else{
        //Otherwise, starts from end
        ret=get_end(fd,sizeof(string))-HEADER_LEN;
    }
    uint32_t prev=ret,cur=ret;
    int64_t len=strlen(src);
    while(len>0){
        if(len<BLOCK_LEN){
            pwrite(fd,zeros,BLOCK_LEN,cur+HEADER_LEN);
            pwrite(fd,src  ,len      ,cur+HEADER_LEN);
        }else{
            pwrite(fd,src  ,BLOCK_LEN,cur+HEADER_LEN);
        }
        src+=BLOCK_LEN;
        len-=BLOCK_LEN;
        if(flag){
            pread(fd,&cur,sizeof(uint32_t),prev+BLOCK_LEN+HEADER_LEN);
            if(len<0){
                pwrite(fd,&prev,sizeof(uint32_t),offsetof(header,backup_prev));
            }
            if(cur==list[1]){
                pwrite(fd,&list[1],sizeof(uint32_t),0);
                cur=get_end(fd,sizeof(string))-HEADER_LEN;
                flag=0;
            }
        }else{
            cur=get_end(fd,sizeof(string))-HEADER_LEN;
        }
        if(len>0){
            write_db(fd, prev+BLOCK_LEN,&cur,sizeof(uint32_t));
        }else{
            if(flag){
                pwrite(fd,&cur,sizeof(uint32_t),0);
            }
            write_db(fd, prev+BLOCK_LEN,zeros,sizeof(uint32_t));
        }
        prev=cur;
    }
    pwrite(fd, (flag?&cur:&list[1])
            ,sizeof(uint32_t),0);
    return ret;
}

static inline void neg_backup(int fd){
    pwrite(fd,zeros,1,offsetof(header,backup_flag));
}
void recov_backup(int fd){
    uint8_t flag=0;
    pread(fd,&flag,sizeof(flag),offsetof(header,backup_flag));
    if(flag){
        header h;
        pread(fd,&h,sizeof(header),0);
        pwrite(fd, &(h.backup_list),sizeof(h.backup_list),0);
        write_db(fd,h.pos                  ,&(h.backup_tab)    ,sizeof(h.backup_tab));
        write_db(fd,h.backup_prev+BLOCK_LEN,&(h.free_list.head),sizeof(h.backup_prev));
        neg_backup(fd);
    }
}

void start_backup(int fd,uint32_t pos){
    tab origin_tab;
    read_db(fd,pos,&origin_tab,sizeof(tab));
    header h={
    //.free_list,
    //.backup_list,
    .backup_tab=origin_tab,
    .pos=pos,
    //.backup_prev;
    .backup_flag=1
    };
    pread(fd,&h.free_list,sizeof(h.free_list),0);
    h.backup_list=h.free_list;
    pwrite(fd,&h,sizeof(h),0);
}

int check_backup(int fd,uint32_t key_pos){
    uint8_t flag=0;
    uint32_t backup_pos;
    pread(fd,&flag,sizeof(flag),offsetof(header,backup_flag));
    if(flag){
        pread(fd,&backup_pos,sizeof(uint32_t),offsetof(header,backup_tab.key));
        if(backup_pos==key_pos){
            return 1;
        }
    }
    return 0;
}
enum{KVDB_RD,KVDB_WR,KVDB_UN};
static void lock_op(volatile int *p,int del){
    static pthread_mutex_t lk=PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&lk);
    *p+=del;
    pthread_mutex_unlock(&lk);
}
static void kvdb_lock(kvdb_t *db,int op){
    int file_op=-1;
    switch(op){
        case KVDB_RD:
            file_op=LOCK_SH;
            int tmp;
            if((tmp=db->wr_acq)>0){
                ++db->rd_cnt;
                if(tmp!=db->wr_acq){
                    db->rd_cnt=0;
                }
            }
            while(db->rd_cnt>0x77);
            pthread_rwlock_rdlock(&db->lk);
            break;
        case KVDB_WR:
            file_op=LOCK_EX;
            lock_op(&db->wr_acq, 1);
            pthread_rwlock_wrlock(&db->lk);
            lock_op(&db->wr_acq,-1);
            db->rd_cnt=0;
            break;
        case KVDB_UN:
            file_op=LOCK_UN;
            pthread_rwlock_unlock(&db->lk);
            break;
        default:
            fprintf(stderr,__FILE__ "%d%s Unrecognized operation",(int)__LINE__,__func__);
    }
    flock(db->fd,file_op);
}

int kvdb_open(kvdb_t *db, const char *filename){
    db->fd=open(filename,O_RDWR|O_CREAT,0777);
    db->rd_block=0;
    db->rd_cnt=0;
    db->wr_acq=0;
    pthread_rwlock_init(&db->lk,NULL);
    if(db->fd<0)return db->fd;
    if(pread(db->fd,useless_buf,HEADER_LEN,0)<HEADER_LEN){
        kvdb_lock(db,KVDB_WR);
        init_db(db->fd);
        kvdb_lock(db,KVDB_UN);
    }
    return 0;
}

int kvdb_close(kvdb_t *db){
    return close(db->fd);
}

static inline int _kvdb_put(int fd, const char *key, const char *value){
    tab cur_tab={.next=0};
    uint32_t cur_off=0;(void)cur_off;
    while(  cur_off=cur_tab.next,
            read_db(fd,cur_tab.next,&cur_tab,sizeof(tab)),
            cur_tab.next!=0){
        string key_str;
        read_db(fd,cur_tab.key,&key_str,sizeof(string));
        if(!string_cmp(key,key_str,fd)){
            uint32_t backup_val=cur_tab.value;
            start_backup(fd,cur_off);
            cur_tab.value=alloc_str(value,fd);
            cur_tab.value_len=strlen(value);
            write_db(fd,cur_off,&cur_tab,sizeof(tab));
            add_free_list(fd,backup_val);
            return 0;
        }
    }
    start_backup(fd,cur_off);
    cur_tab.value=alloc_str(value,fd);
    cur_tab.value_len=strlen(value);
    cur_tab.key=alloc_str(key,fd);
    cur_tab.key_len=strlen(key);
    cur_tab.next=get_end(fd,sizeof(tab))-HEADER_LEN;
    write_db(fd,cur_tab.next,zeros,sizeof(tab));
    write_db(fd,cur_off,&cur_tab,sizeof(tab));
    return 0;
}
int kvdb_put(kvdb_t *db, const char *key, const char *value){
    kvdb_lock(db,KVDB_WR);
    int fd=db->fd;
    recov_backup(fd);
    int ret=_kvdb_put(fd,key,value);
    neg_backup(fd);
    kvdb_lock(db,KVDB_UN);
    return ret;
}

static inline char *_kvdb_get(int fd, const char *key){
    tab cur_tab={.next=0};
    while(read_db(fd,cur_tab.next,&cur_tab,sizeof(tab)),
            cur_tab.next!=0){
        string key_str;
        read_db(fd,cur_tab.key,&key_str,sizeof(string));
        if(!string_cmp(key,key_str,fd)){
            string val_str;
            if(check_backup(fd,cur_tab.key)){
                pread(fd,&cur_tab,sizeof(tab),offsetof(header,backup_tab));
            }
            read_db(fd,cur_tab.value,&val_str,sizeof(string));
            char *ret=malloc(cur_tab.value_len+1);
            string_cpy(ret,val_str,fd);
            return ret;
        }
    }
    return NULL;
}
char *kvdb_get(kvdb_t *db, const char *key){
    kvdb_lock(db,KVDB_RD);
    char *ret=_kvdb_get(db->fd,key);
    kvdb_lock(db,KVDB_UN);
    return ret;
}
void kvdb_traverse(kvdb_t *db){
    tab cur_tab={.next=0};
    int fd=db->fd;
    while(read_db(fd,cur_tab.next,&cur_tab,sizeof(tab)),
            cur_tab.next!=0){
        string key_str,val_str;
        read_db(fd,cur_tab.key,&key_str,sizeof(string));
        printf("Key:  ");
        string_puts(key_str,fd);
        read_db(fd,cur_tab.value,&val_str,sizeof(string));
        printf("Val:  ");
        string_puts(val_str,fd);
    }
}
