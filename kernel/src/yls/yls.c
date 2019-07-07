#include <devices.h>
#include <fs.h>

#define HEADER_LEN 0x100

typedef union yls_node yls_node;
typedef struct yls_dir yls_dir;
typedef struct yls_file yls_file;
typedef struct info info;

inodeops_t yls_iops;

static void yls_init(struct filesystem *fs,const char* name,device_t* dev){
    return;
}
inode_t *yls_lookup(struct filesystem* fs, const char* path, int flags){
    yls_node *cur=pmm->alloc(sizeof(yls_node));
#define read(off,buf,count) fs->dev->ops->read(fs->dev,off,buf,count);

    read(HEADER_LEN,cur,0x10);
    inode_t ret={
        .ptr=cur,
        .fs=fs,
        .ops=&yls_iops
    };
    return ret;
}
int yls_close(inode_t *inode){
    return 0;
}

fsops_t yls_ops={
    .init=yls_init,
    .lookup=yls_lookup,
    .close=yls_close
};

#define YLS_DIR  0
#define YLS_FILE 1
#define YLS_MNT  2

union yls_node{
    struct{
        uint32_t type;
        uint32_t ptr;
        uint32_t name;
    }
    uint8_t align[0x10];
};

struct info{
    uint32_t next;
    char mem[0x40-4];
};

inodeops yls_iops={
    /*
    .open   =yls_iopen,
    .close  =yls_iclose,
    .read   =yls_iread,
    .write  =yls_iwrite,
    .lseek  =yls_ilseek,
    .mkdir  =yls_imkdir,
    .rmdir  =yls_irmdir,
    .link   =yls_ilink,
    .unlink =yls_iunlink
    */
};
