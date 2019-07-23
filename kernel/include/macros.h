#ifndef __MACROS_H__
#define __MACROS_H__


#define winfo(string) \
    (string), strlen((string))

#define rinfo(string) \
    (string), sizeof((string))

#define WRAP(...) \
    do{ \
      __VA_ARGS__  \
    }while(0)

#define TODO() WRAP(Assert(0,"Haven't be realized!\n");)
#define BARRIER() WRAP(Assert(0,"Should not reach here!\n");)

#define MACRO_VALUE(_arg)     #_arg
#define MACRO_SELF(_arg)      _arg
#define MACRO_CONCAT_REAL(_arg1,_arg2) _arg1 ## _arg2
#define MACRO_CONCAT(_arg1,_arg2) MACRO_CONCAT_REAL(_arg1,_arg2)
#define TO_STRING(_arg)     MACRO_VALUE(_arg)

//#define _LOCAL

#ifdef _LOCAL
    #define local_log(...) log(__VA_ARGS__)
#else
    #define local_log(...) 
#endif//_LOCAL

#define LEN(arr) ((sizeof(arr) / sizeof(arr[0])))

#define new(A) (typeof(A)*)pmm->alloc(sizeof(A))

#define TASK_FENCE

#define min(a,b) ((a)>(b)?(b):(a))

#define warn(fmt,...) \
    do { \
        sprintf(get_cur()->err,fmt, ##__VA_ARGS__); \
    }while(0)
    
#define clear_warn() \
    (get_cur()->err[0]='\0',(void)0)

#define error_clear() \
    (get_cur()->err[0]='\0',(void)0)

#define error(fmt,...) \
    do { \
        char warn_str[0x100]; \
        sprintf(warn_str,fmt, ##__VA_ARGS__); \
        warning(warn_str); \
        exit(); \
    }while(0)

#endif//__MACROS_H__
