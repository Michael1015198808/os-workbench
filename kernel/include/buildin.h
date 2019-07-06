#ifndef __BUILDIN_H
#define __BUILDIN_H

typedef int handler(void*[]);

#define helper(name) \
    const char* const help_##name

#define declare(name) \
    handler name; \
    helper(name)
declare(echo)="Write arguments to the standard output.";
declare(cat)="Concatenate FILE(s) to standard output.";

#endif//__BUILDIN_H
