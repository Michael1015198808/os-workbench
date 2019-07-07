#include <common.h>
#include <parse.h>

void panic(char*);
int runcmd(struct cmd *cmd);
inline int run_pipe_cmd(struct cmd *cmd);
inline void backup_fd(vfile_t *backup[3],task_t* current){

// Execute cmd.  Never returns.
int runcmd(struct cmd *cmd){
    struct execcmd *ecmd;

    if(cmd == 0)
        exit();

    switch(cmd->type){
        default:
            panic("command parsing failed!\n");

        case EXEC:
            ecmd = (struct execcmd*)cmd;
            if(ecmd->argv[0] == 0)
                exit();
            vfs->exec(ecmd->argv[0], (void**)(ecmd->argv));
            exit();

        case REDIR:
            TODO();
            /*
            rcmd = (struct redircmd*)cmd;
            close(rcmd->fd);
            if(open(rcmd->file, rcmd->mode) < 0){
                fprintf(2, "open %s failed\n", rcmd->file);
                exit();
            }
            runcmd(rcmd->cmd);
            break;
            */

        case LIST:
            TODO();
            /*
            lcmd = (struct listcmd*)cmd;
            if(fork1() == 0)
            runcmd(lcmd->left);
            wait();
            runcmd(lcmd->right);
            break;
            */

        case PIPE:
            run_pipe_cmd(cmd);
            break;

        case BACK:
            TODO();
            /*
            bcmd = (struct backcmd*)cmd;
            if(fork1() == 0)
            runcmd(bcmd->cmd);
            break;
            */
        }
    Assert(0,"Should not reach here!\n");
    exit();
}
inline int run_pipe_cmd(struct cmd *cmd){
    intr_close();
    int cpu_id=_cpu();
    task_t* current=currents[cpu_id];
    intr_open();

    struct pipecmd* pcmd = (struct pipecmd*)cmd;
    char buf[0x400]={};//Manually 

    vfile_t *backup[3];
    backup_fd(backup,current);

    current->fd[1]->type=VFILE_MEM;
    current->fd[1]->ptr =buf;

    task_t* son=pmm->alloc(sizeof(task_t));
    kmt->create(son,"fork-and-run",(task_fun)runcmd,pcmd->left);
    kmt->teardown(son);

    current->fd[0]->type=VFILE_MEM;
    current->fd[0]->ptr =buf;
    return runcmd(pcmd->right);
}
inline void backup_fd(vfile_t *backup[3],task_t* current){
    for(int i=0;i<3;++i){
        backup[i]=current->fd[i];
        current->fd[i]=pmm->alloc(sizeof(vfile_t));
        current->fd[i]->type=VFILE_NULL;
    }
}
inline void restore_fd(vfile_t *backup[3],task_t* current){
    for(int i=0;i<3;++i){
        pmm->free(current->fd[i]);
        current->fd[i]=backup[i];
    }
}
