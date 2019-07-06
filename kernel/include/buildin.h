#ifndef __BUILDIN_H
#define __BUILDIN_H

typedef int handler(void*[]);
#define declare(function) \
    handler function; \
    extern const char* const MACRO_CONCAT(help_,MACRO_SELF(function))


handler echo; \
extern const char* const help_echo;
declare(cat);

#endif//__BUILDIN_H
