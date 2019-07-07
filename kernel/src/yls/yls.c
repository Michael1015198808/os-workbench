#include <devices.h>
#include <yls.h>
#include <fs.h>

inodeops_t yls_iops;

static void yls_init(struct filesystem *fs,const char* name,device_t* dev){
    return;
}

inode_t *yls_lookup(struct filesystem* fs, const char* path, int flags){
    int path_len=strlen(path)-1;//Starts from "/"
    ++path;

    yls_node *cur=pmm->alloc(sizeof(yls_node));
    fs->dev->ops->read(fs->dev,HEADER_LEN,cur,12);
    cur->cnt=0;

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
    ssize_t ret=0;

    filesystem* fs= ((inode_t*)file->ptr)->fs;
    yls_node* node= ((inode_t*)file->ptr)->ptr;
    switch(node->type){
        case YLS_DIR:
            {
                uint32_t off,nread;
                if(fs->dev->ops->read(fs->dev,node->info,&off,4)!=4)return -1;
                if(node->cnt==7){
                    node->cnt=0;
                    node->info=0;
                    TODO();
                }else{
                    ++node->cnt;
                    node->info+=4;
                }
                if(off==0)return 0;
                off+=8;//To name
                nread=fs->dev->ops->read(fs->dev,node->info,buf,0x40-4);
                ret+=nread;
                return ret;
            }
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
