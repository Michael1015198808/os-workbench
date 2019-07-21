#include "klib.h"
#include <stdarg.h>

#if !defined(__ISA_NATIVE__) || defined(__NATIVE_USE_KLIB__)

static int vsnprintf(char *out, size_t n, const char *fmt, va_list ap);
int puts(const char *s){
    int i=0;
    for(;s[i];++i){
        _putc(s[i]);
    }
    return i;
}
int printf(const char *fmt, ...) {
    va_list ap;
#define MAX 200
    char buf[MAX];
    va_start(ap,fmt);
    int i,n=vsnprintf(buf,-1,fmt,ap);
    assert(n<MAX);
#undef MAX
    static pthread_mutex_t putc_lock=PTHREAD_MUTEX_INITIALIZER;
    intr_close();
    pthread_mutex_lock(&putc_lock);
    for(i=0;i<n;++i){
        _putc(buf[i]);
    }
    pthread_mutex_unlock(&putc_lock);
    intr_open();
    return i;
}

int vsprintf(char *out, const char *fmt, va_list ap) {
    return vsnprintf(out,-1,fmt,ap);
}

int sprintf(char *out, const char *fmt, ...) {
    va_list ap;
    va_start(ap,fmt);
    int ret=vsnprintf(out,-1,fmt,ap);
    return ret;
}

int snprintf(char *out, size_t n, const char *fmt, ...) {
    va_list ap;
    va_start(ap,fmt);
    int ret=vsnprintf(out,n,fmt,ap);
    return ret;
}
//Added by Michael
//Should work as an assistant.

static inline int isalpha(char c){
    return (c>='a'&&c<='z')||
           (c>='A'&&c<='Z');
}
//Wrapped by vsnprintf
inline static int vsnprintf_real(char *out, size_t n, const char *fmt, va_list ap){
#define output(A) \
    if(cnt<n-1){ \
        out[cnt++]=A; \
    }else{ \
        va_end(ap); \
        out[cnt]='\0'; \
        return cnt; \
    }
    size_t cnt=0;
    int i=0;
    const char *p, *sval;
    char fill,num[10];
    int ival,fill_width;
    double dval;
    uintptr_t uval;
    char cval;
    for(p=fmt;*p!='\0';++p){
        if(*p!='%'){
            output(*p);
            continue;
        }
        fill_width=0;
        fill=' ';
        //do {
            if (*p == '0') {
            fill = '0';
            ++p;
            }
            fill_width = 0;
            ++p;
            while (*p >= '0' && *p <= '9') {
                fill_width *= 10;
                fill_width += *p - '0';
                ++p;
            }
            switch (*p) {
                case 'c':
                    cval=va_arg(ap,int);
                    output(cval);
                    break;
                case 'u':
                    uval = va_arg(ap, uint32_t);
                    i = 0;
                    while (uval > 0) {
                        num[i++] = uval % 10 + '0';
                        uval /= 10;
                    }
                    while (fill_width > i) {
                        num[i++] = fill;
                    }
                    if (i == 0) {
                        output('0');
                    } else {
                        while (i > 0) {
                            output(num[--i]);
                        }
                    }
                    break;
                case 'x':
                case 'p':
                    uval = (uintptr_t) va_arg(ap, void * );
                    i = 8;
                    while (i > 0) {
                        output(
                                (uval >> ((sizeof(void *) << 3) - 4)) < 10 ?
                                (uval >> ((sizeof(void *) << 3) - 4)) + '0' :
                                (uval >> ((sizeof(void *) << 3) - 4)) - 10 + 'a');
                        uval <<= 4;
                        --i;
                    }
                    break;
                case 'd':
                    ival = va_arg(ap, int);
                    if (ival < 0) {
                        output('-');
                        ival = -ival;
                    } while (ival > 0) {
                        num[i++] = ival % 10 + '0';
                        ival /= 10;
                    }
                    while (fill_width > i) {
                        num[i++] = fill;
                    }
                    if (i == 0) {
                        output('0');
                    } else {
                        while (i > 0) {
                            output(num[--i]);
                        }
                    }
                    break;
                case 'f':{
                    dval = va_arg(ap, double);
                    double d=1;
                    if(dval>1){
                        while(dval>d)d*=10;
                        d/=10;
                        while(d>0.9){
                            int j = (int)(dval / d);
                            output(j + '0');
                            dval -= j * d;
                            d /= 10;
                        }
                        output('.');
                    }
                    while (d > 0.001) {
                        int j = (int)(dval / d);
                        output(j + '0');
                        dval -= j * d;
                        d /= 10;
                    }
                    break;
                }
                case 's':
                    for (sval = va_arg(ap, char * ); *sval != '\0'; ++sval) {
                        output(*sval);
                    }
                    break;
                default:
                    if (isalpha(*p)) {
                        _putc(*p);
                        for(char* c="Not realized";*c;++c){
                            _putc(*c);
                        }
                        assert(0);
                    }else{
                        output(*p);
                    }
            }
        //}while(0);
    }
    va_end(ap);
    output('\0');
    return cnt-1;
#undef output
}

//work as a wrapper 
static int vsnprintf(char *out, size_t n, const char *fmt, va_list ap){
    /*
    static pthread_mutex_t io_lk=PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&io_lk);
    */
    int ret=vsnprintf_real(out,n,fmt,ap);
    /*
    pthread_mutex_unlock(&io_lk);
    */
    return ret;
}
#endif
