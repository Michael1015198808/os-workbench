//Borrowed from xv6

#include <common.h>
#include <parse.h>
#include <dir.h>

void panic(char*);
int runcmd(struct cmd *cmd);
static inline void run_redir_cmd(struct cmd* cmd);
static inline void run_pipe_cmd (struct cmd* cmd);
static inline void run_back_cmd (struct cmd* cmd);
/*
inline void backup_fd(vfile_t *backup[3],task_t* current);
inline void restore_fd(vfile_t *backup[3],task_t* current);
*/

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
            break;

        case REDIR:
            run_redir_cmd(cmd);
            break;

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
    return -1;
}
static inline void run_redir_cmd(struct cmd* cmd){
    struct redircmd* rcmd = (struct redircmd*)cmd;
    vfs->close(rcmd->fd);
    char dest[0x100];
    to_absolute(dest,get_pwd(),rcmd->file);
    if(vfs->open(dest, rcmd->mode) < 0){
        fprintf(2, "open %s failed\n", rcmd->file);
        exit();
    }
    runcmd(rcmd->cmd);
}

static inline void run_pipe_cmd(struct cmd *cmd){
    task_t* current=get_cur();

    struct pipecmd* pcmd = (struct pipecmd*)cmd;

    vfile_t *backup;
    int pipefd[2];
    extern void pipe(int pipefd[2]);
    pipe(pipefd);

    backup        =current->fd[1];
    current->fd[1]=current->fd[pipefd[1]];
    current->fd[pipefd[1]]=NULL;

    task_t* son=pmm->alloc(sizeof(task_t));
    kmt->create(son,"fork-and-run",(task_fun)runcmd,pcmd->left);
    current->fd[1]=backup;
    kmt->wait(son);

    current->fd[0]=current->fd[pipefd[0]];
    current->fd[pipefd[0]]=NULL;

    runcmd(pcmd->right);
}

static inline void run_back_cmd(struct cmd* cmd){
    struct backcmd* bcmd = (struct backcmd*)cmd;
    kmt->create(pmm->alloc(sizeof(task_t)),NULL,(task_fun)runcmd,bcmd->cmd);
}
