#include <common.h>
#include <klib.h>

#define copy_name(dest,src) \
    dest=pmm->alloc(strlen(src)+1); \
    memcpy(dest,src,strlen(src)+1);

task_t *tasks[20]={};
//static spinlock_t tasks_lk;
static pthread_mutex_t tasks_lk;
int tasks_idx=0,tasks_old=0;
char tasks_log[66000];
#define trace_pthread_mutex_lock(_lk) \
    pthread_mutex_lock(_lk); \
    detail_log(tasks_log,tasks_idx,"lock"); \

#define trace_pthread_mutex_unlock(_lk) \
    detail_log(tasks_log,tasks_idx,"unlock"); \
    pthread_mutex_unlock(_lk);

/* tasks, tasks_cnt
 * shared by
 *   add_task
 *   remove_task
 *   kmt_context_save
 *   kmt_context_switch
 */
int currents[4]={-1,-1,-1,-1},tasks_cnt=0;
#define current currents[cpu_id]

void show_sem_list(sem_t *sem){
    int p;
    for(p=sem->head;p!=sem->tail;++p){
        printf("%s->",sem->pool[p]->name);
    }
    printf("%s->",sem->pool[p]->name);
}

void show(){
    extern sem_t echo_sem;
    show_sem_list(&echo_sem);
    printf("%s",tasks[0]->name);
    for(int i=1;i<tasks_cnt;++i){
        printf("->%s",tasks[i]->name);
    }
    printf("\n");
}
static void add_task(task_t *task){
    pthread_mutex_lock(&tasks_lk);
    tasks[tasks_cnt++]=task;
    pthread_mutex_unlock(&tasks_lk);
}

#define set_flag(A,B) \
    { \
        pthread_mutex_lock(&A->attr_lock); \
        uintptr_t p=(uintptr_t)&A->attr; \
        asm volatile("lock or %1,(%0)"::"r"(p),"g"((B))); \
        pthread_mutex_unlock(&A->attr_lock); \
    }

#define neg_flag(A,B) \
    { \
        pthread_mutex_lock(&A->attr_lock); \
        uintptr_t p=(uintptr_t)&A->attr; \
        asm volatile("lock and %1,(%0)"::"r"(p),"g"(~(B))); \
        pthread_mutex_unlock(&A->attr_lock); \
    }

static _Context* kmt_context_save(_Event ev, _Context *c){
    //trace_pthread_mutex_lock(&tasks_lk);
    int cpu_id=_cpu();
    if(current==-1){
        current=tasks_cnt-1;
        //kmt->create(pmm->alloc(sizeof(task_t)),"os_run",NULL,NULL);
        //current=tasks_cnt-1;
    }else{
        tasks[current]->context=*c;
    }
    //trace_pthread_mutex_unlock(&tasks_lk);
    return NULL;
}
static _Context* kmt_context_switch(_Event ev, _Context *c){
    trace_pthread_mutex_lock(&tasks_lk);
    int cpu_id=_cpu(),new=0;
    //log("context switch from (%d)%s\n",current,tasks[current]->name);
    new=current;
    uint16_t cnt=0;
    do{
        //current=rand()%tasks_cnt;
        ++new;
        ++cnt;
        new%=tasks_cnt;
        if(cnt==0){
            trace_pthread_mutex_unlock(&tasks_lk);
            //if((tasks[current]->attr&TASK_SLEEP)==0)return &tasks[current]->context;
            for(volatile uint32_t sleep=1;sleep<10000000;++sleep);//Sleep if can't get any process to run
            trace_pthread_mutex_lock(&tasks_lk);
        }
    }while(tasks[new]->attr);

    tasks[current]->cpu=-1;
    neg_flag(tasks[current],TASK_RUNNING);

    tasks[new]->cpu=cpu_id;
    set_flag(tasks[new],TASK_RUNNING);

    current=new;
    trace_pthread_mutex_unlock(&tasks_lk);
    for(int i=0;i<4;++i){
        if(tasks[current]->fence1[i]!=0x13579ace||tasks[current]->fence2[i]!=0xeca97531){
            log("Stack over/under flow!\n");
            while(1);
        }
    }
    return &tasks[current]->context;
}
void kmt_init(void){
    os->on_irq(INT_MIN, _EVENT_NULL, kmt_context_save);
    os->on_irq(INT_MAX, _EVENT_NULL, kmt_context_switch);
    //kmt->spin_init(&tasks_lk, "tasks-lock");
}
int kmt_create(task_t *task, const char *name, void (*entry)(void *arg), void *arg){
    static int ignore_num=2;
    if(ignore_num>0){
        --ignore_num;
        return 0;
    }
    //task->id=tasks_cnt;
    log("create (%d)%s\n",tasks_cnt,name);
    add_task(task);
    Assert(tasks_cnt<LEN(tasks),"%d\n",tasks_cnt);
    task->cpu=-1;
    task->attr=TASK_RUNABLE;
    task->attr_lock=0;
    copy_name(task->name,name);

    task->context = *_kcontext(
            (_Area){(void*)task->stack,&(task->stack_end)}, entry, arg);
#ifdef TASK_FENCE
    for(int i=0;i<4;++i){
        task->fence1[i]=0x13579ace;
        task->fence2[i]=0xeca97531;
    }
#endif
    return 0;
}
void kmt_teardown(task_t *task){
    Assert(0);
    pmm->free(task->name);
    return ;
}
void kmt_spin_init(spinlock_t *lk, const char *name){
    lk->locked=PTHREAD_MUTEX_INITIALIZER;
    copy_name(lk->name,name);
}
static pthread_mutex_t inner_lock=PTHREAD_MUTEX_INITIALIZER;
//static char inner_log[66000];
//static int inner_idx=0;
void kmt_spin_lock(spinlock_t *lk){
    int cpu_id=_cpu();(void)cpu_id;
    pthread_mutex_lock(&inner_lock);
    /*
    while(1){
        if(lk->locked){
            if(lk->owner==_cpu()){
                ++lk->reen;
                break;
            }else{
                while(lk->locked){
                    pthread_mutex_unlock(&inner_lock);
                    intr_log("open");
                    intr_open();
                    _yield();
                    //while(1);
                    intr_close();
                    intr_log("close");
                    for(volatile int i=0;i<10000;++i);
                    pthread_mutex_lock(&inner_lock);
                };
            }
        }
        pthread_mutex_lock(&lk->locked);
        lk->reen=1;
        lk->owner=_cpu();
        intr_close();
        intr_log("close");
        break;
    }//Use break to release lock and restore intr
    */
    pthread_mutex_unlock(&inner_lock);
}
void kmt_spin_unlock(spinlock_t *lk){
    pthread_mutex_lock(&inner_lock);
    /*
    if(lk->locked){
        if(lk->owner!=_cpu()){
            log("Lock[%s] isn't holded by this CPU!\n",lk->name);
        }else{
            if(lk->reen==1){
                lk->owner=-1;
                //True but sometimes slow
                pthread_mutex_unlock(&(lk->locked));
            }else{
                --lk->reen;
            }
        }
    }else{
        Assert(0,"Lock[%s] isn't locked!\n",lk->name);
    }
    */
    pthread_mutex_unlock(&inner_lock);
}
void kmt_sem_init(sem_t *sem, const char *name, int value){
    copy_name(sem->name,name);
    sem->value=value;
    kmt->spin_init(&(sem->lock),name);
    sem->head=0;
    sem->tail=0;
    //log("%s: %d",sem->name,sem->value);
}


char addrm_log[66000];
volatile uint16_t addrm_idx=0;

static void sem_add_task(sem_t *sem){
    int cpu_id=_cpu();
    addrm_idx+=sprintf(addrm_log+addrm_idx,"add:[%d]:%x",sem->tail,tasks[current]);
    
    sem->pool[sem->tail++]=tasks[current];
    set_flag(tasks[current],TASK_SLEEP);
    addrm_idx+=sprintf(addrm_log+addrm_idx,"(%d)\n",tasks[current]->attr);
    if(sem->tail>=POOL_LEN)sem->tail-=POOL_LEN;

    kmt->spin_unlock(&(sem->lock));
    _yield();
}
static void sem_remove_task(sem_t *sem){
    addrm_idx+=sprintf(addrm_log+addrm_idx,"remove:[%d]:%x",sem->head,sem->pool[sem->head]);

    neg_flag(sem->pool[sem->head++],TASK_SLEEP);
    addrm_idx+=sprintf(addrm_log+addrm_idx,"(%d)\n",sem->pool[(sem->head+19)%20]->attr);
    if(sem->head>=POOL_LEN)sem->head-=POOL_LEN;
}

void kmt_sem_wait(sem_t *sem){
    kmt->spin_lock(&(sem->lock));
    --(sem->value);

    if(sem->value<0){
        return sem_add_task(sem);
    }
    kmt->spin_unlock(&(sem->lock));
}
void kmt_sem_signal(sem_t *sem){
    kmt->spin_lock(&(sem->lock));
    ++(sem->value);

    if(sem->value<=0){
        sem_remove_task(sem);
    }
    kmt->spin_unlock(&(sem->lock));
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
  .sem_signal  =kmt_sem_signal
};
