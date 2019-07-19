#include <common.h>
#include <devices.h>
#include <vfs.h>
#include <yls.h>
#include <dir.h>
/* blkfs:
 * vfile----offset
 *      |---refcnt /ptr(yls_node)
 *      \---inode---fs(blkfs)
 *                 \ops(blkfs_iops)
 */
static void blkfs_init(filesystem* fs,const char* name,device_t* dev){
    fs->name  =name;
    fs->dev   =dev;
    fs->inodes=pmm->alloc(sizeof(inode_t)*(INODE_END-INODE_START)/0x8);
    fs->root  =fs->inodes;

    for(int i=0;INODE_START+i*sizeof(yls_node)<INODE_END;++i){
        fs->inodes[i].ptr=pmm->alloc(16);
        fs->dev->ops->read(fs->dev,
                INODE_START+i*sizeof(yls_node),
                fs->inodes[i].ptr,
                sizeof(yls_node));
        fs->inodes[i].fs=fs;
        fs->inodes[i].ops=fs->inodeops;
    }
}

static inode_t* blkfs_lookup(filesystem* fs,const char* path,int flags){
    if((!path[0]))return fs->root;
    return vfs->find(fs->root,path,flags);
}

static int blkfs_close(inode_t* inode){
    return 0;
}

static fsops_t blkfs_ops={
    .init  =blkfs_init,
    .lookup=blkfs_lookup,
    .close =blkfs_close,
};

static int blkfs_iopen(vfile_t* file,int flags){
    if(((yls_node*)file->inode->ptr)->type==YLS_DIR){
        //First 4 bytes indicates the parent directory in my file system
        file->offset=4;
    }else{
        file->offset=0;
    }
    file->flags=flags;
    file->refcnt=1;
    file->lk=PTHREAD_MUTEX_INITIALIZER;
    return 0;
}

static int blkfs_iclose(vfile_t* file){
    pthread_mutex_lock(&file->lk);
    --file->refcnt;
    if(file->refcnt==0){
        return file->inode->fs->ops->close(file->inode);
    }
    pthread_mutex_unlock(&file->lk);
    return 0;
}

static ssize_t inline blkfs_iread(vfile_t* file,char* buf,size_t size){
    filesystem* fs  = file->inode->fs;
    uint32_t fd_off = file->offset;
    yls_node* node  = file->inode->ptr;
    uint32_t off    = node->info;
    uint32_t fsize  = node->size;

    switch(node->type){
        case YLS_DIR:
            return EISDIR;
            break;
        case YLS_FILE:
            {
                if(fd_off>size){
                    return 0;
                }else if(size+fd_off>fsize){
                    size=fsize-fd_off;
                }
                ssize_t nread=block_read(fs->dev,off,fd_off,buf,size);
                file->offset+=nread;
                return nread;
            }
            break;
    }
    BARRIER();
    return 0;
}

static ssize_t inline blkfs_ireaddir(vfile_t* file,char* buf,size_t size){
    ssize_t ret=0;

    filesystem* fs  =file->inode->fs;
    uint32_t fd_off =file->offset;
    yls_node* node  =file->inode->ptr;
    uint32_t off=node->info;

    switch(node->type){
        case YLS_DIR:
            {
                if(block_read(fs->dev,off,fd_off,(void*)&off,4)!=4)return 0;
                if(off==0)return 0;//find_block failed or reached the end

                ssize_t nread=block_read(fs->dev,off,4,buf,size);
                ret+=nread;
                file->offset+=4;
                return ret;
            }
            break;
        case YLS_FILE:
            error("Try to readdir on a file");
            break;
    }
    BARRIER();
    return 0;
}

static ssize_t blkfs_iwrite(vfile_t* file,const char* buf,size_t size){
    filesystem* fs  = file->inode->fs;
    uint32_t fd_off = file->offset;
    yls_node* node  = file->inode->ptr;
    uint32_t off    = node->info;
    uint32_t fsize  = node->size;

    if(off==0&&fsize==0){
        node->info=off=new_block(fs->dev);
    }
    switch(node->type){
        case YLS_DIR:
            return EISDIR;
            break;
        case YLS_FILE:
            {
                ssize_t nwrite=block_write(fs->dev,off,fd_off,buf,size);
                file->offset+=nwrite;
                if(file->offset>fsize){
                    node->size=file->offset;
                }
                return nwrite;
            }
            break;
        default:
            TODO();
    }
    Assert(0,"Should not reach here");
}

static off_t blkfs_ilseek(vfile_t* file,off_t offset,int whence){
    switch(whence){
        case SEEK_SET:
            return file->offset=offset;
        case SEEK_CUR:
            return file->offset+=offset;
        case SEEK_END:
            TODO();
    }
    BARRIER();
}

static inode_t* blkfs_ifind(inode_t* cur,const char* path,int flags){
    inode_t* next=NULL;

    const filesystem* fs=cur->fs;
    device_t* dev=fs->dev;
    yls_node* node=cur->ptr;
    uint32_t offset=node->info;

    ssize_t(*const read )(device_t*,off_t,      void*,size_t)=fs->dev->ops->read;
    ssize_t(*const write)(device_t*,off_t,const void*,size_t)=fs->dev->ops->write;

    uint32_t id=-1;//id of inode

    if(path[0]=='.'){
        if(path[1]=='.'){
            //.. for parent
            if(cur==cur->fs->root){
                next=cur->fs->root_parent;
            }else{
                uint32_t off;
                read(dev,offset,&off,4);
                read(dev,off,&id,4);
                next=fs->inodes+id;
            }
            path+=2;
        }else{
            //. for current
            next=cur;
            ++path;
        }
    }else{
        while(offset){
            char layer[0x100];
            int layer_len=get_first_layer(path);
            strncpy(layer,path,layer_len);
            uint32_t blk_off;
            if(read(fs->dev,offset,&blk_off,4)!=4||!blk_off){
                //No more file in this directory
                if( (flags&O_CREATE) && (path[layer_len]=='\0')){
                    uint32_t off=new_block(fs->dev),inode=new_inode(fs->dev);
                    write(fs->dev,offset,&off,4);
                    log("  off:%x\ninode:%x\n",off,inode);
                    id=(inode-INODE_START)/0x10;
                    *(yls_node*)fs->inodes[id].ptr=(yls_node){
                        .refcnt=1,
                        .info  =0,
                        .size  =0,
                        .type  =YLS_FILE,
                    };
                    write(fs->dev,off,&id,4);
                    write(fs->dev,off+4,path,strlen(path));
                    return fs->inodes+id;
                }else{
                    warn("No such file or directory");
                    return NULL;
                }
            };
            if(block_cmp(fs->dev,blk_off,layer)){
                //Mismatch
                offset+=4;
                if(offset%BLK_SZ==BLK_MEM){
                    //Find next offset
                    read(fs->dev,offset,&offset,4);
                }
            }else{
                report_if(1);
                path+=layer_len;
                read(fs->dev,blk_off,&id,4);
                next=fs->inodes+id;
                break;
            }
        }
    }
    /*
    if(id!=-1){
        if((((yls_node*)fs->inodes[id].ptr)->type!=YLS_DIR)){
            //Not a directory
            if(((flags&O_DIRECTORY)||path[0]=='/')){
                //./file/ can be opened iff file is a directory
                warn("Not a directory");
            }
        }else{
            if(flags&O_WRONLY){
                //Not a directory
                warn("Is a directory");
            }
        }
    }
    */

    return vfs->find(next,path,flags);
}
static inodeops_t blkfs_iops={
    .open   =blkfs_iopen,
    .close  =blkfs_iclose,
    .read   =blkfs_iread,
    .readdir=blkfs_ireaddir,
    .write  =blkfs_iwrite,
    .lseek  =blkfs_ilseek,
    /*
    .mkdir  =blkfs_imkdir,
    .rmdir  =blkfs_rmdir,
    .link   =blkfs_ilink,
    .unlink =blkfs_iunlink,
    */
    .find   =blkfs_ifind,
};

filesystem blkfs[2]={
    {
        .name    =NULL,
        .ops     =&blkfs_ops,
        .inodeops=&blkfs_iops,
    },
    {
        .name    =NULL,
        .ops     =&blkfs_ops,
        .inodeops=&blkfs_iops,
    },
};
