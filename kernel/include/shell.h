#ifndef __SHELL_H
#define __SHELL_H

typedef int handler(void*[]);

#define helper(name) \
    const char* const help_##name

handler echo;
handler cat;

int exec_buildin(const char* file,void* args[],int *is_buildin);

struct cmd;
void runcmd(struct cmd* cmd);
struct cmd* parsecmd(char *s);
#endif//__SHELL_H
