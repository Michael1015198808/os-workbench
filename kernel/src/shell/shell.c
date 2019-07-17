#include <common.h>
#include <amtrace.h>
#include <klib.h>
#include <devices.h>
#include <shell.h>

static void fork_and_run(void *input){
        runcmd(parsecmd((char*)input));
}
void mysh(void *name) {
    task_t* cur=get_cur();
    {
        for(int i=0;i<3;++i){
            char *std_name[3]={"stdin","stdout","stderr"};
            int temp=vfs->open(name,7);
            Assert(temp==i,"fd of %s should be %d, instead of %d!\n",std_name[i],i,temp);
        }
    }
    while (1) {
        char input[0x100], prompt[0x100];
        sprintf(prompt, "(%s) [%s] $ ", name,cur->pwd);
        int nread;
        do{
            std_write(prompt);
            nread=std_read(input);
        }while(nread==0);
        input[nread-1]='\0';
        printf("%s\n",input);
        if(strncmp("cd",input,2)||(input[2]!='\0'||input[2]!=' ')){
            task_t* son=pmm->alloc(sizeof(task_t));
            kmt->create(son,"mysh",fork_and_run,input);
            kmt->wait(son);
        }else{
            input[2]='\0';
            void* cd_args[]={input,input+3};
            mysh_cd(cd_args);
        }
    }
}
