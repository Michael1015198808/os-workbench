#include <common.h>
#include <vfs.h>
#include <yls.h>
#include <dir.h>

typedef struct{
    volatile int head,tail,open;
    void* mem;
}pipe_t;

static inodeops_t pipe_iops;
int pipe(int pipefd[2]) {
    pipe_t* p=pmm->alloc(sizeof(pipe_t));
    p->head =0;
    p->tail =0;
    p->open =1;
    p->mem  =pmm->alloc(0x100);

    inode_t* inode=pmm->alloc(sizeof(inode_t));
    inode->ptr=p;
    inode->fs =NULL;
    inode->ops=&pipe_iops;


    task_t* cur=get_cur();
    for(int i=0;i<2;++i){
        int new_fd_num(task_t*);
        pipefd[i]=new_fd_num(cur);

        vfile_t* file=pmm->alloc(sizeof(vfile_t));
        file->offset=0;
        file->inode =inode;
        file->refcnt=1;
        file->flags =0777;
        file->lk    =0;

        cur->fd[pipefd[i]]=file;
    }
    return 0;
}

static inline size_t get_size(pipe_t* p){
    return (p->tail-p->head)&0xff;
}
static inline int is_full(pipe_t* p){
    return get_size(p)==0xff;
}
static inline int is_empty(pipe_t* p){
    return get_size(p)==0;
}


static ssize_t pipe_read(pipe_t* p,void *buf, size_t count) {
    size_t nread=0;

    size_t size=0;
    while((size=get_size(p))==0){
        if(!p->open){return 0;}
        _yield();
    }

    if(count>size){
        //not enough
        return pipe_read(p,buf,size);
    }else if(count+p->head>0x100){
        //circular
        int remain=0x100-p->head;
        nread +=pipe_read(p,buf,remain);
        p->head=0;
        nread +=pipe_read(p,buf+remain,count-remain);
    }else{
        memcpy(buf,p->mem+p->head,count);
        p->head +=count;
        nread   +=count;
    }

    return nread;
}

static ssize_t pipe_write(pipe_t* p, const void *buf, size_t count) {
    size_t nwrite=0;

    size_t size;
    while((size=get_size(p))==0xff)_yield();

    if(count+size>0xff){
        //full
        int remain=0xff-size;
        nwrite  +=pipe_write(p,buf,remain);
        nwrite  +=pipe_write(p,buf+remain,count-remain);
    }else if(count+p->tail>0x100){
        //circular
        int remain=0x100-p->tail;
        nwrite  +=pipe_write(p,buf,remain);
        p->tail  =0;
        nwrite  +=pipe_write(p,buf+remain,count-remain);
    }else{
        memcpy(p->mem+p->tail,buf,count);
        p->tail +=count;
        nwrite  +=count;
    }
    return nwrite;
}
static int pipe_iclose(vfile_t* file){
    pipe_t* p=file->inode->ptr;
    return p->open=0;
}

static ssize_t pipe_iread(vfile_t* file,char* buf,size_t size){
    return pipe_read(file->inode->ptr,buf,size);
}
static ssize_t pipe_iwrite(vfile_t* file,const char* buf,size_t size){
    return pipe_write(file->inode->ptr,buf,size);
}

static inodeops_t pipe_iops={
    .close  =pipe_iclose,
    .read   =pipe_iread,
    .write  =pipe_iwrite,
};
