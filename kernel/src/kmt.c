#include <common.h>
#include <klib.h>
#include <vfs.h>

#define copy_name(dest,src) \
    (dest=pmm->alloc(strlen(src)+1), \
    strcpy(dest,src) )

task_t *tasks[0x40]={},*currents[MAX_CPU]={},*lasts[MAX_CPU],idles[MAX_CPU];
static pthread_mutex_t tasks_lk,free_task_lk=PTHREAD_MUTEX_INITIALIZER;
char tasks_log[66000];
int tasks_idx=0;
#define trace_pthread_mutex_lock(_lk) \
    pthread_mutex_lock(_lk);

#define trace_pthread_mutex_unlock(_lk) \
    pthread_mutex_unlock(_lk);

/* tasks,
 * shared by
 *   add_task
 *   remove_task
 *   kmt_context_save
 *   kmt_context_switch
 */
#define current currents[cpu_id]
#define last lasts[cpu_id]

static inline int add_task_real(task_t *task){
    static int pid=0;

    do{
        ++pid;
        pid&=0x3f;
    }while(tasks[pid]);
    tasks[pid]=task;
    task->pid=pid;
    return 0;
}
static int add_task(task_t* task){
    pthread_mutex_lock(&tasks_lk);
    add_task_real(task);
    pthread_mutex_unlock(&tasks_lk);
    return 0;
}

#define set_flag(A,B) \
    { \
        uintptr_t p=(uintptr_t)&A->attr; \
        asm volatile("lock or %1,(%0)"::"r"(p),"g"((B))); \
    }

#define neg_flag(A,B) \
    { \
        uintptr_t p=(uintptr_t)&A->attr; \
        asm volatile("lock and %1,(%0)"::"r"(p),"g"(~(B))); \
    }

static _Context* kmt_context_save(_Event ev, _Context *c){
    pthread_mutex_lock(&tasks_lk);
    pthread_mutex_unlock(&tasks_lk);//Similar to rw-lock
    int cpu_id=_cpu();
    if(last&&current!=last){
        pthread_mutex_unlock(&last->running);
    }
    last=current;
    if(current){
        current->context=*c;
        current->ncli=ncli[cpu_id];
        current->intena=intena[cpu_id];
    }
    return NULL;
}
//int log_idx=0;
//char log[120000]={};

static inline _Context* kmt_context_switch_real(_Event ev, _Context *c){
    int cpu_id=_cpu(),new=-1;
    Assert(_intr_read()==0,"%d",cpu_id);
    int cnt=10000;

    do{
        new=rand()%0x40;
        --cnt;
        if(cnt==0){
            Assert(_intr_read()==0,"%d",cpu_id);
            current=NULL;
            return &idles[cpu_id].context;
        }
    }while((tasks[new]->attr & ~TASK_NOWAIT) ||
           pthread_mutex_trylock(&tasks[new]->running));

    current=tasks[new];

    for(int i=0;i<4;++i){
        if(current->fence1[i]!=0x13579ace||current->fence2[i]!=0xeca97531){
            log("Stack over/under flow!\ntask: (%d)%s\n",new,current->name);
            while(1);
        }
    }
    ncli[cpu_id]=current->ncli;
    intena[cpu_id]=current->intena;
    return &tasks[new]->context;
}

static _Context* kmt_context_switch(_Event ev, _Context *c){
    _Context* ret=kmt_context_switch_real(ev,c);
    return ret;
}

void idle(void *arg){
    while(1)_yield();
}

pool ctx_queue={
    .lk=0,
    .head=0,
    .tail=0,
};

static _Context* kmt_context_clean(_Event ev, _Context *c){
    task_t* task=dequeue(&ctx_queue);
    if(task){
        if(pthread_mutex_trylock(&task->running))enqueue(&ctx_queue,task);
        else{
            kmt->teardown(task);
            pmm->free(task);
        }
    }
    return NULL;
}

void kmt_init(void){
    os->on_irq(INT_MIN, _EVENT_NULL, kmt_context_save);
    os->on_irq(INT_MAX, _EVENT_NULL, kmt_context_switch);
    os->on_irq(INT_MAX, _EVENT_NULL, kmt_context_clean);
    for(int i=0;i<_ncpu();++i){
        idles[i].attr=TASK_RUNABLE;
        idles[i].running=0;
        idles[i].context= *_kcontext(
            (_Area){
            (void*)idles[i].stack,
            &(idles[i].stack_end)
            }, idle, NULL);
    }
}

int kmt_create(task_t *task, const char *name, void (*entry)(void*), void *arg){
    static int ignore_num=0;
    if(ignore_num>0){
        --ignore_num;
        return 0;
    }
    *task=(task_t){
        .attr   =TASK_RUNABLE,
        .ncli   =0,
        .intena =0,
        .pid    =0,//Handle later
        .name   =NULL,//Handle later
        .running=0,
    };
    copy_name(task->name,name);
    task_t* cur=get_cur();

    if(cur){
        for(int i=0;i<FD_NUM;++i){
            if(cur->fd[i]){
                task->fd[i]=cur->fd[i];
                pthread_mutex_lock(&cur->fd[i]->lk);
                ++cur->fd[i]->refcnt;
                pthread_mutex_unlock(&cur->fd[i]->lk);
            }
        }
        strcpy(task->pwd,cur->pwd);
        task->cur_dir=cur->cur_dir;
    }else{
        strcpy(task->pwd,"/");
        /*
        int tempfd=vfs->open(task->pwd,O_RDONLY);
        if(tempfd<0){
            error("Can't open / for working directory.");
        }
        task->cur=task->fd[tempfd]->inode;
        vfs->close(tempfd);
        */
    }

    task->context = *_kcontext(
            (_Area){
            (void*)task->stack,
            &(task->stack_end)
            }, entry, arg);
#ifdef TASK_FENCE
    for(int i=0;i<4;++i){
        task->fence1[i]=0x13579ace;
        task->fence2[i]=0xeca97531;
    }
#endif
    Assert(add_task(task)==0,"Create task %s failed!\n",name);
    local_log("create (%d)%s\n",task->pid,name);
    return 0;
}

//All security check should be done by caller
//This function directly clear the task
void kmt_teardown(task_t *task){
    pmm->free(task->name);
}

void kmt_spin_init(spinlock_t *lk, const char *name){
    lk->locked=PTHREAD_MUTEX_INITIALIZER;
    copy_name(lk->name,name);
}

pthread_mutex_t exclu_lk=PTHREAD_MUTEX_INITIALIZER;
void kmt_spin_lock(spinlock_t *lk){
    intr_close();
    int cpu_id=_cpu();
    while(1){
        if(lk->locked){
            if(lk->owner==current){
                ++lk->reen;
                break;
            }else{
                while(lk->locked);
            }
        }
        pthread_mutex_lock(&lk->locked);
        intr_close();
        lk->reen=1;
        lk->owner=current;
        break;
    }//Use break to release lock and restore intr
    intr_open();
}

void kmt_spin_unlock(spinlock_t *lk){
    int cpu_id=_cpu();
    if(lk->locked){
        if(lk->owner!=current){
            local_log("Lock[%s] isn't held by this routine!\n",lk->name);
        }else{
            if(--lk->reen==0){
                lk->owner=NULL;
                pthread_mutex_unlock(&(lk->locked));
                intr_open();
            }
        }
    }else{
        Assert(0,"Lock[%s] isn't locked!\n",lk->name);
    }
}

void kmt_sem_init(sem_t *sem, const char *name, int value){
    copy_name(sem->name,name);
    sem->value=value;
    //kmt->spin_init(&(sem->lock),name);
    sem->lock=PTHREAD_MUTEX_INITIALIZER;
    sem->head=0;
    sem->tail=0;
    //log("%s: %d",sem->name,sem->value);
}

static void sem_add_task(sem_t *sem){
    int cpu_id=_cpu();
    task_t* park=current;

    sem->pool[sem->tail++]=park;
    set_flag(park,TASK_SLEEP);
    if(sem->tail>=POOL_LEN)sem->tail-=POOL_LEN;

    pthread_mutex_unlock(&(sem->lock));
    intr_open();
    _yield();
}

static void sem_remove_task(sem_t *sem){

    neg_flag(sem->pool[sem->head],TASK_SLEEP);
    if(++sem->head>=POOL_LEN)sem->head-=POOL_LEN;
}

void kmt_sem_wait_real(sem_t *sem){
    intr_close();
    pthread_mutex_lock(&(sem->lock));

    if(--sem->value<0){
        return sem_add_task(sem);
    }
    pthread_mutex_unlock(&(sem->lock));
    intr_open();
}

void kmt_sem_wait(sem_t *sem){
    kmt_sem_wait_real(sem);
}

void kmt_sem_signal_real(sem_t *sem){
    intr_close();
    pthread_mutex_lock(&(sem->lock));

    if(++sem->value<=0){
        sem_remove_task(sem);
    }
    pthread_mutex_unlock(&(sem->lock));
    intr_open();
}
void kmt_sem_signal(sem_t *sem){
    kmt_sem_signal_real(sem);
}

void inline exit_real(task_t* cur){
    TODO();
}

void warning(const char* warn){
    task_t* cur=get_cur();
    fprintf(2,"%s: %s\n",cur->name,warn);
}

void exit(void){
    intr_close();
    task_t* cur=get_cur();
    for(int i=0;i<FD_NUM;++i){
        if(cur->fd[i]){
            vfs->close(i);
        }
    }
    set_flag(cur,TASK_ZOMBIE);
    tasks[cur->pid]=NULL;
    if(cur->attr&TASK_NOWAIT){
        enqueue(&ctx_queue,cur);
    }
    _yield();
}

void kmt_wait(task_t* task){
    while(!(task->attr&TASK_ZOMBIE))_yield();
    pthread_mutex_lock(&task->running);
}

MODULE_DEF(kmt) {
  .init        =kmt_init,
  .create      =kmt_create,
  .teardown    =kmt_teardown,
  .spin_init   =kmt_spin_init,
  .spin_lock   =kmt_spin_lock,
  .spin_unlock =kmt_spin_unlock,
  .sem_init    =kmt_sem_init,
  .sem_wait    =kmt_sem_wait,
  .sem_signal  =kmt_sem_signal,
  .wait        =kmt_wait,
};
