#ifndef __SHELL_H
#define __SHELL_H

#define handler(name) int mysh_##name(void*[]);

handler(echo);
handler(cat);
handler(help);
handler(cd);
handler(wc);
handler(ls);
handler(mkdir);
handler(rmdir);
handler(sleep);
handler(touch);
handler(test);
handler(link);
handler(unlink);
handler(rm);
handler(mv);
handler(cp);
handler(color);

int exec_buildin(const char* file,void* args[],int *is_buildin);

struct cmd;
int runcmd(struct cmd* cmd);
struct cmd* parsecmd(char *s);

#endif//__SHELL_H
