//Borrowed from xv6

#include <common.h>
#include <parse.h>
#include <dir.h>
#include <vfs.h>

#define panic(...) \
    do{ \
        fprintf(2,__VA_ARGS__); \
        exit(); \
    }while(0)

extern void fork_and_run(void *input);
int runcmd(struct cmd *cmd);
static inline void run_redir_cmd(struct cmd* cmd);
static inline void run_list_cmd (struct cmd* cmd);
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
            if(ecmd->argv[0])
                vfs->exec(ecmd->argv[0], (void**)(ecmd->argv));
            break;

        case REDIR:
            run_redir_cmd(cmd);
            break;

        case LIST:
            run_list_cmd(cmd);
            break;

        case PIPE:
            run_pipe_cmd(cmd);
            break;

        case BACK:
            run_back_cmd(cmd);
            break;
        }
    pmm->free(cmd);
    return 0;
}

static inline void run_redir_cmd(struct cmd* cmd){
    struct redircmd* rcmd = (struct redircmd*)cmd;
    vfs->close(rcmd->fd);
    char dest[0x100];
    to_absolute(dest,get_pwd(),rcmd->file);
    if(vfs->open(dest, rcmd->mode) != rcmd->fd){
        exit();
    }
    runcmd(rcmd->cmd);
}

static inline void run_list_cmd(struct cmd* cmd){
    struct listcmd* lcmd = (struct listcmd*)cmd;
    task_t* son=pmm->alloc(sizeof(task_t));
    kmt->create(son,"fork-and-run",(task_fun)fork_and_run,lcmd->left);

    kmt->wait(son);
    kmt->teardown(son);
    pmm->free(son);

    runcmd(lcmd->right);
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

    task_t* sons=pmm->alloc(sizeof(task_t)*2);
    kmt->create(sons+0,"fork-and-run",(task_fun)fork_and_run,pcmd->left);
    current->fd[1]=backup;

    current->fd[0]=current->fd[pipefd[0]];
    current->fd[pipefd[0]]=NULL;
    kmt->create(sons+1,"fork-and-run",(task_fun)fork_and_run,pcmd->right);

    kmt->wait(sons+0);
    kmt->teardown(sons+0);
    kmt->wait(sons+1);
    kmt->teardown(sons+1);
    pmm->free(sons);
}

static inline void run_back_cmd(struct cmd* cmd){
    struct backcmd* bcmd = (struct backcmd*)cmd;
    task_t* son=pmm->alloc(sizeof(task_t));
    kmt->create(son,"fork-and-run",(task_fun)fork_and_run,bcmd->cmd);
    pthread_mutex_lock(&son->running);
    if((son->attr&TASK_ZOMBIE)){
        kmt->teardown(son);
        pmm->free(son);
    }else{
        uintptr_t p=(uintptr_t)&son->attr;
        asm volatile("lock or %1,(%0)"::"r"(p),"g"(TASK_NOWAIT));
        pthread_mutex_unlock(&son->running);
    }
}
