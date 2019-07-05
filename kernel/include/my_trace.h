#ifndef __MY_TRACE_H
#define __MY_TRACE_H

    //#define spinlock_log
    
    //#define sem_log(A,info,...) A->idx&=(1<<16)-1;A->idx+=sprintf(A->log+A->idx,"\n[cpu%d]%s:%d %s:%s %d",_cpu(),__func__,__LINE__,tasks[currents[_cpu()]]->name,  #info, A->value)
    #ifndef sem_log
        #define sem_log(...) 
    #endif
    
    //#define intr_log(info) {int cpu_id=_cpu();pthread_mutex_lock(&log_lk);(intr_idx_+=sprintf(intr_log_+intr_idx_,"\n[cpu%d]%s:%d %s(%d):%s %d",cpu_id,__FILE__,__LINE__,tasks[currents[cpu_id]]->name,currents[cpu_id],info,ncli[cpu_id]));pthread_mutex_unlock(&log_lk);intr_idx_&=(1<<16)-1;}
    #define detail_log(_log,_idx,info) \
        do{ \
            int cpu_id=_cpu(); \
            _idx+=sprintf(_log+_idx,"\n[cpu%d]%s:%3d(%s) %s:%s",_cpu(),__FILE__,__LINE__,__func__,tasks[current]->name,info); \
            _idx&=(1<<16)-1; \
        }while(0)
#else
    #define sem_log
    #define detail_log
#endif


#ifdef intr_log
    char intr_log_[66000];
    int intr_idx_;
    pthread_mutex_t log_lk;
#else
    #define intr_log(...)
#endif

#ifndef detail_log
#define detail_log(...)

#endif//__MY_TRACE_H
