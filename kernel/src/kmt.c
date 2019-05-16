#include <common.h>
#include <klib.h>

#define copy_name(dest,src) \
    dest=pmm->alloc(strlen(src)+1); \
    memcpy(dest,src,strlen(src)+1);

static task_t *tasks[20]={};
static spinlock_t tasks_lk;
/*
 * shared by
 *   add_task
 *   remove_task
 *   kmt_context_save
 *   kmt_context_switch
 */
int currents[4]={-1,-1,-1,-1},tasks_cnt=0;
#define current currents[cpu_id]
void kmt_test(){
    void *tmp=tasks[1];
    tasks[1]=tasks[4];
    tasks[4]=tmp;
}
void show_sem_list(sem_t *sem){
    list_t *p;
    for(p=sem->head;p!=sem->tail;p=p->next){
        printf("%s->",p->task->name);
    }
    printf("%s\n",p->task->name);
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

static int add_task(task_t *task){
    kmt->spin_lock(&tasks_lk);
    int tmp=tasks_cnt;
    tasks[tasks_cnt++]=task;
    kmt->spin_unlock(&tasks_lk);
    return tmp;
}
void remove_task(){
    kmt->spin_lock(&tasks_lk);
    log("%d %d\n",currents[_cpu()],tasks_cnt);
    tasks[currents[_cpu()]]=tasks[--tasks_cnt];
    kmt->spin_unlock(&tasks_lk);
}
static _Context* kmt_context_save(_Event ev, _Context *c){
    kmt->spin_lock(&tasks_lk);
    int cpu_id=_cpu();
    if(current==-1){
        current=tasks_cnt-1;
        kmt->spin_unlock(&tasks_lk);
        return NULL;
        kmt->create(pmm->alloc(sizeof(task_t)),"os_run",NULL,NULL);
        current=tasks_cnt-1;
    }
    //log("context save\n");
    tasks[current]->context=*c;
    tasks[current]->cpu=-1;
    kmt->spin_unlock(&tasks_lk);
    return NULL;
}
static _Context* kmt_context_switch(_Event ev, _Context *c){
    kmt->spin_lock(&tasks_lk);
    int cpu_id=_cpu();
    extern int *switch_flag;
    switch_flag[cpu_id]=1;
    //log("context switch from (%d)%s\n",current,tasks[current]->name);
    do{
        //current=rand()%tasks_cnt;
        ++current;
        current%=tasks_cnt;
    }while(tasks[current]->cpu!=cpu_id&&tasks[current]->cpu>=0);
    //printf(" %d ",tasks_cnt);
    tasks[current]->cpu=cpu_id;
    //log("context switch to (%d)%s\n",current,tasks[current]->name);
    kmt->spin_unlock(&tasks_lk);
    return &tasks[current]->context;
}
void kmt_init(void){
    os->on_irq(INT_MIN, _EVENT_NULL, kmt_context_save);
    os->on_irq(INT_MAX, _EVENT_NULL, kmt_context_switch);
    kmt->spin_init(&tasks_lk, "tasks-lock");
}
int kmt_create(task_t *task, const char *name, void (*entry)(void *arg), void *arg){
    static int ignore_num=2;
    if(ignore_num>0){
        --ignore_num;
        return 0;
    }
    //task->id=tasks_cnt;
    log("create (%d)%s\n",add_task(task),name);
    Assert(tasks_cnt<LEN(tasks),"%d\n",tasks_cnt);
    task->cpu=-1;
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
void kmt_spin_lock(spinlock_t *lk){
    static pthread_mutex_t inner_lock=PTHREAD_MUTEX_INITIALIZER;
    intr_close();
    pthread_mutex_lock(&inner_lock);
    while(1){
        if(lk->locked){
            if(lk->owner==_cpu()){
                ++lk->reen;
                break;
            }else{
                while(lk->locked){
                    pthread_mutex_unlock(&inner_lock);
                    intr_open();
                    _yield();
                    intr_close();
                    pthread_mutex_lock(&inner_lock);
                };
            }
        }
        pthread_mutex_lock(&lk->locked);
        lk->reen=1;
        lk->owner=_cpu();
        break;
    }//Use break to release lock and restore intr
    pthread_mutex_unlock(&inner_lock);
    intr_open();
}
void kmt_spin_unlock(spinlock_t *lk){
    if(lk->locked){
        if(lk->owner!=_cpu()){
            log("Lock[%s] isn't holded by this CPU!\n",lk->name);
        }else{
            if(lk->reen==1){
                lk->owner=-1;
                _intr_write(lk->int_on);
                //True but sometimes slow
                pthread_mutex_unlock(&(lk->locked));
            }else{
                --lk->reen;
            }
        }
    }else{
        log("Lock[%s] isn't locked!\n",lk->name);
        while(1);
    }
}
void kmt_sem_init(sem_t *sem, const char *name, int value){
    copy_name(sem->name,name);
    sem->value=value;
    sem->capa=10000;
    kmt->spin_init(&(sem->lock),name);
    sem->head=NULL;
    sem->tail=pmm->alloc(sizeof(list_t));
    //log("%s: %d",sem->name,sem->value);
}
static void sem_add_task(sem_t *sem){
    int cpu_id=_cpu();
    sem->tail->next=pmm->alloc(sizeof(list_t));
    sem->tail=sem->tail->next;
    sem->tail->task=tasks[current];
    sem->tail->next=NULL;
    if(sem->head==NULL){
        sem->head=sem->tail;
    }
    remove_task();
    _yield();
}
static void sem_remove_task(sem_t *sem){
    add_task(sem->head->task);
    Assert(sem->head!=NULL);
    sem->head=sem->head->next;
}
void kmt_sem_wait(sem_t *sem){
    kmt->spin_lock(&(sem->lock));
    --(sem->value);
    if(sem->value>sem->capa){
        sem_remove_task(sem);
    }else if(sem->value<=0){
        return sem_add_task(sem);
    }
    kmt->spin_unlock(&(sem->lock));
}
void kmt_sem_signal(sem_t *sem){
    kmt->spin_lock(&(sem->lock));
    ++(sem->value);
    if(sem->value>sem->capa){
        return sem_add_task(sem);
    }else if(sem->value<0){
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
