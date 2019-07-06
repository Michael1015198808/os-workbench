#include <common.h>
#include <amtrace.h>
#include <klib.h>
#include <devices.h>
#include <shell.h>

static void fork_and_run(void *input){
        runcmd(parsecmd((char*)input));
}
void mysh(void *name) {
    {
        for(int i=0;i<3;++i){
            char *std_name[3]={"stdin","stdout","stderr"};
            int temp=vfs->open(name,7);
            Assert(temp==i,"fd of %s should be %d, instead of %d!\n",std_name[i],i,temp);
        }
    }
    while (1) {
        char input[0x100], prompt[0x100];
        sprintf(prompt, "(%s) $ ", name);
        std_write(prompt);
        std_read(input);
        intr_close();
        int cpu_id=_cpu();
        task_t* son=pmm->alloc(sizeof(task_t));
        for(int i=0;i<3;++i){
            son->fd[i]=currents[cpu_id]->fd[i];
        }
        intr_open();
        kmt->create(son,"fork-and-run",fork_and_run,input);
        kmt->teardown(son);
    }
}
