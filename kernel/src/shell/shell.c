#include <common.h>
#include <amtrace.h>
#include <klib.h>
#include <devices.h>
#include <shell.h>

void mysh(void *name) {
    {
        for(int i=0;i<3;++i){
            char *std_name[3]={"stdin","stdout","stderr"};
            int temp=vfs->open(name,7);
            Assert(temp==i,"fd of %s should be %d, instead of %d!\n",std_name[i],i,temp);
        }
    }
    while (1) {
        char input[128], prompt[128];
        void *args[10];
        sprintf(prompt, "(%s) $ ", name);
        std_write(prompt);
        int nread=std_read(input);
        runcmd(parsecmd(buf));
    }
}