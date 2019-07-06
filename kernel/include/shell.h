#ifndef __SHELL_H
#define __SHELL_H

typedef int handler(void*[]);

#define helper(name) \
    const char* const help_##name

#define declare(name) \
    handler name; \
    helper(name)
declare(echo)="Write arguments to the standard output.";
declare(cat)="Concatenate FILE(s) to standard output.";

int exec_buildin(const char* file,void* args[],int *is_buildin);

struct cmd;
void runcmd(struct cmd* cmd);
struct cmd* parsecmd(char *s);
#endif//__SHELL_H
