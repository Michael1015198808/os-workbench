#ifndef __BUILDIN_H
#define __BUILDIN_H

typedef int handler(void*[]);
#define declare(function) \
    handler(function); \
    const char* const MACRO_CONCAT(help_,MACRO_SELF(command));
declare(echo);
declare(cat);

#endif//__BUILDIN_H
