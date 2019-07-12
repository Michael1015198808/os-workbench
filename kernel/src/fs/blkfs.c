#include <common.h>
#include <devices.h>
#include <vfs.h>
#include <yls.h>
#include <dir.h>
/* blkfs:
 * vfile----offset
 *      |---refcnt /ptr(yls_node)
 *      \---inode---fs(blkfs)
 *                 \ops()
 */
static void blkfs_init(filesystem* fs,const char* name,device_t* dev){
    fs->name=name;
    fs->dev=dev;
}

static inode_t* blkfs_lookup(filesystem* fs,const char* path,int flags){
    Assert(path[0]=='/',"Absolute path should start with /\n");
    ++path;
    ssize_t(*const read)(device_t*,off_t,void*,size_t)=fs->dev->ops->read;

    yls_node *cur=pmm->alloc(sizeof(yls_node));
    inode_t* ret =pmm->alloc(sizeof(inode_t));
    ret->ptr     =cur;
    ret->fs      =fs;
    ret->ops     =fs->inodeops;
    read(fs->dev,HEADER_LEN,cur,12);

    int pos=find_path(fs->dev,ret,path);

    if(path[pos]!='\0'){//Look up failed at middle
        task_t* current=get_cur();
        fprintf(2,"%s: %s: No such file or directory\n",current->name,path);
        exit();
    }

    return ret;
}

static int blkfs_close(inode_t* inode){
    TODO();
}

static fsops_t blkfs_ops={
    .init  =blkfs_init,
    .lookup=blkfs_lookup,
    .close =blkfs_close,
};

ssize_t inline blkfs_iread_real(vfile_t* file,char* buf,size_t size){
    ssize_t ret=0;

    filesystem* fs  =file->inode->fs;
    uint64_t offset =file->offset;
    yls_node* node  =file->inode->ptr;

    switch(node->type){
        case YLS_DIR:
            {
                //Read a directory, you'll get
                //names of directorys in it
                //(So readdir is not needed)
                const uint32_t sz=0x40-4;//Size of information per block
                uint32_t off=node->info;
                for(offset =file->offset;offset>sz;offset-=sz){
                    if(fs->dev->ops->read(fs->dev,node->info+sz,&off,4)!=4)return 0;
                }
                if(fs->dev->ops->read(fs->dev,off+offset,&off,4)!=4)return 0;
                if(off==0)return 0;
                fs->dev->ops->read(fs->dev,off+8,&off,4);
                nread=fs->dev->ops->read(fs->dev,off,buf,sz);
                ret+=nread;
                file->offset+=4;
                return ret;
            }
        case YLS_FILE:
            TODO();
            break;
    }
    Assert(0,"Should not reach here!\n");
    return 0;
}
ssize_t inline blkfs_iread(vfile_t* file,char* buf,size_t size){
    ssize_t ret=blkfs_iread_real(file,buf,size);
    return ret;
}
ssize_t blkfs_iwrite(vfile_t* file,const char* buf,size_t size){
    filesystem* fs= file->inode->fs;
    yls_node* node= file->inode->ptr;

    switch(node->type){
        case YLS_DIR:
            {
                //Write to a dir will create
                //a file in it with name $buf
                uint32_t parent_info=node->info;
                uint32_t type=((uint32_t*)buf)[0];
                const char* path=((void**)buf)[1];
                while(*path){
                    uint32_t next=new_block(fs->dev,0x10);
                    yls_node new_node;
                    new_node.type=type;
                    new_node.info=new_block(fs->dev,0x40);
                    new_node.name=new_block(fs->dev,0x40);
                    int pos=get_first_layer(path);

                    fs->dev->ops->write(fs->dev,find_end(fs->dev,parent_info),&next,12);//Parent's info
                    fs->dev->ops->write(fs->dev,next,&new_node,12);//Son's tab
                    fs->dev->ops->write(fs->dev,new_node.name,path,pos);//Son's name
                    path+=pos;
                    if(*path=='/')++path;
                    parent_info=new_node.info;
                }
                return size;
            }
            break;
        default:
            TODO();
    }
    Assert(0,"Should not reach here");
}
static inodeops_t blkfs_iops={
    /*
    .open  =blkfs_iopen,
    .close =blkfs_iclose,
    */
    .read  =blkfs_iread,
    .write =blkfs_iwrite,
};

filesystem blkfs[2]={
    {
        .ops     =&blkfs_ops,
        .inodeops=&blkfs_iops,
    },
    {
        .ops     =&blkfs_ops,
        .inodeops=&blkfs_iops,
    },
};
