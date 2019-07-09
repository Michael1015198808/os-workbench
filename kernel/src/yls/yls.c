#include <devices.h>
#include <dir.h>
#include <fs.h>
#include <yls.h>

inodeops_t yls_iops;

static void yls_init(struct filesystem *fs,const char* name,device_t* dev){
    return;
}

inode_t *yls_lookup(struct filesystem* fs, const char* path, int flags){
    Assert(path[0]=='/',"Absolute path should start with /\n");
    ++path;
    ssize_t(*const read)(device_t*,off_t,void*,size_t)=fs->dev->ops->read;

    yls_node *cur=pmm->alloc(sizeof(yls_node));
    inode_t* ret=pmm->alloc(sizeof(inode_t));
    ret->ptr    =cur;
    ret->fs     =fs;
    ret->ops    =&yls_iops;
    ret->offset =0;
    read(fs->dev,HEADER_LEN,cur,12);

    int pos=find_path(fs->dev,ret,path);

    if(path[pos]!='\0'){//Look up failed at middle
        task_t* current=get_cur();
        fprintf(2,"%s: %s: No such file or directory\n",current->name,path);
        exit();
    }

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

ssize_t yls_iread(vfile_t* file,char* buf,size_t size){
    ssize_t ret=0;

    filesystem* fs= ((inode_t*)file->ptr)->fs;
    yls_node* node= ((inode_t*)file->ptr)->ptr;

    switch(node->type){
        case YLS_DIR:
            {
                uint32_t off,nread;
                if(fs->dev->ops->read(fs->dev,node->info,&off,4)!=4)return -1;
                if(off==0)return 0;
                if(node->cnt==OFFS_PER_MEM){
                    node->cnt=0;
                    node->info=0;
                    TODO();
                }else{
                    ++node->cnt;
                    node->info+=4;
                }
                off+=8;//To name
                fs->dev->ops->read(fs->dev,off,&off,4);
                nread=fs->dev->ops->read(fs->dev,off,buf,0x40-4);
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
ssize_t yls_iwrite(vfile_t* file,const char* buf,size_t size){
    filesystem* fs= ((inode_t*)file->ptr)->fs;
    yls_node* node= ((inode_t*)file->ptr)->ptr;

    switch(node->type){
        case YLS_DIR:
            {
                uint32_t type=((uint32_t*)buf)[0];
                const char* path=((void**)buf)[1];
                while(*path){
                    uint32_t next=new_block(fs->dev,0x10);
                    yls_node new_node;
                    new_node.type=type;
                    new_node.info=new_block(fs->dev,0x40);
                    new_node.name=new_block(fs->dev,0x40);
                    fs->dev->ops->write(fs->dev,next,&new_node,12);
                    int pos=get_first_layer(path);
                    fs->dev->ops->write(fs->dev,new_node.name,path,pos-1);
                    path+=pos;
                }
                return size;
            }
            break;
        default:
            TODO();
    }
    Assert(0,"Should not reach here");
}
inodeops_t yls_iops={
    /*
    .open   =yls_iopen,
    .close  =yls_iclose,
    */
    .read   =yls_iread,
    .write  =yls_iwrite,
    /*
    .lseek  =yls_ilseek,
    .mkdir  =yls_imkdir,
    .rmdir  =yls_irmdir,
    .link   =yls_ilink,
    .unlink =yls_iunlink
    */
};
