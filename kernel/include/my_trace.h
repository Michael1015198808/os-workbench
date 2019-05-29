#ifndef _MY_TRACE_H
#define _MY_TRACE_H

#define spinlock_log

#ifndef sem_log
    #define sem_log(...) 
#endif

//#define intr_log(info) {int cpu_id=_cpu();pthread_mutex_lock(&log_lk);(intr_idx_+=sprintf(intr_log_+intr_idx_,"\n[cpu%d]%s:%d %s(%d):%s %d",cpu_id,__FILE__,__LINE__,tasks[currents[cpu_id]]->name,currents[cpu_id],info,ncli[cpu_id]));pthread_mutex_unlock(&log_lk);intr_idx_&=(1<<16)-1;}
#define detail_log(_log,_idx,info) \
    do{ \
        int cpu_id=_cpu(),_old=_idx; \
        _idx+=sprintf(_log+_idx,"\n[cpu%d]%s:%3d(%s) %s:%s",_cpu(),__FILE__,__LINE__,__func__,tasks[current]->name,info); \
        (void)_old; \
        tasks_old=_old; \
        /*printf(_log+_old);*/ \
        _idx&=(1<<16)-1; \
    }while(0)

#endif


#ifdef intr_log
    char intr_log_[66000];
    int intr_idx_;
    pthread_mutex_t log_lk;
#else
    #define intr_log(...)
#endif
