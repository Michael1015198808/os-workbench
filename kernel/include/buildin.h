#ifndef __BUILDIN_H
#define __BUILDIN_H

typedef int handler(void*[]);
#define declare(function) \
    handler function; \
    extern char* MACRO_CONCAT(help_,MACRO_SELF(function))

//declare(echo);
declare(cat);

#endif//__BUILDIN_H
