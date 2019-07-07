#include <devices.h>
#include <yls.h>
#include <fs.h>

inodeops_t yls_iops;

static void yls_init(struct filesystem *fs,const char* name,device_t* dev){
    return;
}

#define read(off,buf,count) fs->dev->ops->read(fs->dev,off,buf,count);
inode_t *yls_lookup(struct filesystem* fs, const char* path, int flags){
    int path_len=strlen(path)-1;//Starts from "/"
    ++path;

    yls_node *cur=pmm->alloc(sizeof(yls_node));
    read(HEADER_LEN,cur,0x10);

    while(path_len>0){
        TODO();
    }

    inode_t* ret=pmm->alloc(sizeof(inode_t));
    inode_t tmp={
        .ptr=cur,
        .fs=fs,
        .ops=&yls_iops,
        .offset=0
    };
    *ret=tmp;
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

ssize_t yls_iread(vfile_t *file,char* buf, size_t size){
    ssize_t nread=0;

    yls_node* node= ((inode_t*)file->ptr)->ptr;
    switch(node->type){
        case YLS_DIR:
            nread=0;
            return nread;
        case YLS_FILE:
            TODO();
            break;
    }
    Assert(0,"Should not reach here!\n");
    return 0;
}
inodeops_t yls_iops={
    /*
    .open   =yls_iopen,
    .close  =yls_iclose,
    */
    .read   =yls_iread,
    /*
    .write  =yls_iwrite,
    .lseek  =yls_ilseek,
    .mkdir  =yls_imkdir,
    .rmdir  =yls_irmdir,
    .link   =yls_ilink,
    .unlink =yls_iunlink
    */
};
