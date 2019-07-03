#include <common.h>
#include <klib.h>

#define copy_name(dest,src) \
    (dest=pmm->alloc(strlen(src)+1), \
    strcpy(dest,src) )

task_t *tasks[40]={};
static pthread_mutex_t tasks_lk;
char tasks_log[66000];
int tasks_idx=0;
#define trace_pthread_mutex_lock(_lk) \
    pthread_mutex_lock(_lk);

#define trace_pthread_mutex_unlock(_lk) \
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
    printf("\n");
}

void show(){
    printf("%s",tasks[0]->name);
    for(int i=1;i<tasks_cnt;++i){
        printf("->%s",tasks[i]->name);
    }
    printf("\n");
}

static int add_task(task_t *task){
    pthread_mutex_lock(&tasks_lk);
    int ret=tasks_cnt;
    tasks[tasks_cnt++]=task;
    pthread_mutex_unlock(&tasks_lk);
    return ret;
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
    int cpu_id=_cpu();
    if(current!=-1){
        tasks[current]->context=*c;
    }
    return NULL;
}
//int log_idx=0;
//char log[120000]={};

static _Context* kmt_context_switch(_Event ev, _Context *c){
    int cpu_id=_cpu(),new=current;
    trace_pthread_mutex_lock(&tasks_lk);
    Assert(_intr_read()==0,"%d",cpu_id);
    int cnt=10000;

    do{
        //current=rand()%tasks_cnt;
        ++new;
        --cnt;
        if(new>=tasks_cnt){new=0;}
        if(cnt==0){
            trace_pthread_mutex_unlock(&tasks_lk);
            Assert(_intr_read()==0,"%d",cpu_id);
            if((tasks[current]->attr&TASK_SLEEP)==0)
                return NULL;
            cnt=10000;
            trace_pthread_mutex_lock(&tasks_lk);
        }
    }while(tasks[new]->attr);

    if(current>=0){
        neg_flag(tasks[current],TASK_RUNNING);
    }

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
}
int kmt_create(task_t *task, const char *name, void (*entry)(void*), void *arg){
    static int ignore_num=0;
    if(ignore_num>0){
        --ignore_num;
        return 0;
    }
    //task->id=tasks_cnt;
    local_log("create (%d)%s\n",tasks_cnt,name);
    int task_idx=add_task(task);
    Assert(tasks_cnt<LEN(tasks),"%d\n",tasks_cnt);
    task->attr=TASK_RUNABLE;
    task->attr_lock=0;
    copy_name(task->name,name);

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
    return task_idx;
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

pthread_mutex_t exclu_lk=PTHREAD_MUTEX_INITIALIZER;
void kmt_spin_lock(spinlock_t *lk){
    intr_close();
    int cpu_id=_cpu();
    while(1){
        if(lk->locked){
            if(lk->owner==cpu_id){
                ++lk->reen;
                break;
            }else{
                while(lk->locked);
                /*
                for(volatile int i=0;lk->locked;++i){
                    if(i==10000){
                        i=0;
                        intr_open();
                        _yield();
                        intr_close();
                    }
                }
                */
            }
        }
        pthread_mutex_lock(&lk->locked);
        intr_close();
        lk->reen=1;
        lk->owner=cpu_id;
        break;
    }//Use break to release lock and restore intr
    intr_open();
}

void kmt_spin_unlock(spinlock_t *lk){
    if(lk->locked){
        if(lk->owner!=_cpu()){
            local_log("Lock[%s] isn't held by this CPU!\n",lk->name);
            report_if(1);
        }else{
            if(--lk->reen==0){
                lk->owner=-1;
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

    sem->pool[sem->tail++]=tasks[current];
    set_flag(tasks[current],TASK_SLEEP);
    if(sem->tail>=POOL_LEN)sem->tail-=POOL_LEN;

    pthread_mutex_unlock(&(sem->lock));
    //while(tasks[current]->attr&TASK_SLEEP);
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
