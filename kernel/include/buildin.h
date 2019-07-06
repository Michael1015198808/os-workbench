#ifndef __BUILDIN_H
#define __BUILDIN_H

typedef int handler(void*[]);
#define declare(function) \
    handler(function); \
    const char* const help_##_function;
declare(echo);
declare(cat);

#endif//__BUILDIN_H
