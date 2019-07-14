#include <common.h>
#include <parse.h>

void panic(char*);
int runcmd(struct cmd *cmd);
static inline int run_pipe_cmd(struct cmd *cmd);
static inline int run_back_cmd(struct cmd *cmd);
inline void backup_fd(vfile_t *backup[3],task_t* current);
inline void restore_fd(vfile_t *backup[3],task_t* current);

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
            panic("exec failed!");

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
            run_back_cmd(cmd);
            break;
        }
    exit();
}
static inline int run_pipe_cmd(struct cmd *cmd){
    TODO();
    /*
    intr_close();
    int cpu_id=_cpu();
    task_t* current=currents[cpu_id];
    intr_open();

    struct pipecmd* pcmd = (struct pipecmd*)cmd;

    vfile_t *backup[3];
    int pipefd[2];
    extern void pipe(int pipefd[2]);
    pipe(pipefd);
    backup_fd(backup,current);

    task_t* son=pmm->alloc(sizeof(task_t));
    kmt->create(son,"fork-and-run",(task_fun)runcmd,pcmd->left);
    kmt->teardown(son);

    restore_fd(backup,current);

    return runcmd(pcmd->right);
    */
}
static inline int run_back_cmd(struct cmd* cmd){
    struct backcmd* bcmd = (struct backcmd*)cmd;
    kmt->create(pmm->alloc(sizeof(task_t)),NULL,runcmd,bcmd->cmd);
}
inline void backup_fd(vfile_t *backup[3],task_t* current){
    for(int i=0;i<3;++i){
        backup[i]=current->fd[i];
        current->fd[i]=NULL;
    }
}
inline void restore_fd(vfile_t *backup[3],task_t* current){
    for(int i=0;i<3;++i){
        pmm->free(current->fd[i]);
        current->fd[i]=backup[i];
    }
}
