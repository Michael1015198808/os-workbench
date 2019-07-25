#include <devices.h>
#include <common.h>
#include <dir.h>
#include <vfs.h>
#include <yls.h>
#include <fs.h>
/* blkfs:
 * vfile----offset
 *      |---refcnt /ptr(yls_node)
 *      \---inode---fs(blkfs)
 *                 \ops(blkfs_iops)
 */

const uint32_t yls_size=sizeof(yls_node);

static inodeops_t blkfs_iops;
static void blkfs_init(filesystem* fs,const char* name,device_t* dev){
    fs->name  =name;
    fs->dev   =dev;
    fs->inodes=pmm->alloc(sizeof(inode_t)*(INODE_END-INODE_START)/yls_size);
    fs->root  =fs->inodes;

    uint32_t check=0;
    dev->ops->read(dev,0,&check,4);
    if((check&0xff)<0xff){
        printf("%s is empty, initializing...\n",name);
        //Empty ramdisk
        if(yls_init(dev)){
            fprintf(2,"Something wrong happened when initializing %s\n",name);
        }
    }

    for(int i=0;INODE_START+i*yls_size<INODE_END;++i){
        fs->inodes[i].ptr=pmm->alloc(yls_size);
        fs->dev->ops->read(fs->dev,
                INODE_START+i*yls_size,
                fs->inodes[i].ptr,
                yls_size);
        fs->inodes[i].fs=fs;
        fs->inodes[i].ops=&blkfs_iops;
    }
}

static inode_t* blkfs_lookup(filesystem* fs,const char* path,int flags){
    if((!path[0]))return fs->root;
    return vfs_find(fs->root,path,flags);
}

static int blkfs_close(inode_t* inode){
    return 0;
}

static fsops_t blkfs_ops={
    .init  =blkfs_init,
    .lookup=blkfs_lookup,
    .close =blkfs_close,
};

static inline void update_size(vfile_t* file,uint32_t newsize){
    filesystem* fs=file->inode->fs;
    device_t* dev =fs->dev;
    uint32_t  id  =file->inode-fs->inodes;

    ((yls_node*)file->inode->ptr)->size=newsize;
    dev->ops->write(dev,INODE_START+id*sizeof(yls_node)+offsetof(yls_node,size),&newsize,4);
}

static int blkfs_iopen(vfile_t* file,int flags){
    file->lk=PTHREAD_MUTEX_INITIALIZER;
    pthread_mutex_lock(&file->lk);
    if(((yls_node*)file->inode->ptr)->type==YLS_DIR){
        //First 4 bytes indicates the parent directory in my file system
        file->offset=4;
    }else{
        file->offset=0;
        if(  (flags & O_WRONLY)&&
            !(flags & O_RDONLY)&&
            !(flags & O_APPEND) ){
            update_size(file,0);
        }
    }
    file->flags=flags;
    file->refcnt=1;
    pthread_mutex_unlock(&file->lk);
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
#define fd_off (file->offset)
    yls_node* node  =file->inode->ptr;
    uint32_t off=node->info;

    switch(node->type){
        case YLS_DIR:
            {
                uint32_t dir;
                do{
                    if(block_read(fs->dev,off,fd_off,(void*)&dir,4)!=4)return 0;
                    if(dir==0)return 0;//find_block failed or reached the end
                    fd_off+=4;
                }while(dir==YLS_WIPE);

                ssize_t nread=block_read(fs->dev,dir,4,buf,size);
                ret+=nread;
                return ret;
            }
            break;
        case YLS_FILE:
            error("Try to readdir on a file");
            break;
    }
    BARRIER();
    return 0;
#undef fd_off
}

static ssize_t blkfs_iwrite(vfile_t* file,const char* buf,size_t size){
    filesystem* fs  = file->inode->fs;
    uint32_t fd_off;
    if(file->flags&O_APPEND){
        fd_off = file->inode->ops->lseek(file,0,SEEK_END);
    }else{
        fd_off = file->offset;
    }
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
                    update_size(file,file->offset);
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
            {
                yls_node* node  = file->inode->ptr;
                uint32_t fsize  = node->size;
                return file->offset=(offset+fsize);
            }
    }
    BARRIER();
}

static int blkfs_irmdir(inode_t* parent,const char* name){
    const filesystem* fs=parent->fs;
    device_t* dev=fs->dev;
    yls_node* node=parent->ptr;
    uint32_t offset=node->info;//Skip parent

    ssize_t(*const read )(device_t*,off_t,      void*,size_t)=dev->ops->read;
    ssize_t(*const write)(device_t*,off_t,const void*,size_t)=fs->dev->ops->write;

    if(name[0]=='.'){
        return -1;
    }else{
        offset+=4;
        while(offset){
            uint32_t blk_off;
            if(read(dev,offset,&blk_off,4)!=4||!blk_off){
                //No more file in this directory
                warn("No such file or directory");
                return -1;
            }
            if(blk_off==YLS_WIPE||block_cmp(dev,blk_off,name)){
                //Mismatch
                offset+=4;
                if(offset%BLK_SZ==BLK_MEM){
                    //Find next offset
                    read(dev,offset,&offset,4);
                }
            }else{
                uint32_t id,type;
                read(dev,blk_off,&id,4);
                read(dev,INODE_START+id*sizeof(yls_node)+offsetof(yls_node,type),&type,4);
                if(type!=YLS_DIR){
                    warn("Not a directory!");
                    return -1;
                }else{
                    //TODO: refcnt
                    uint32_t wipe=YLS_WIPE;
                    write(dev,offset,&wipe,4);
                    return 0;
                }
            }
        }
    }

    return -1;
}

static inline uint32_t get_id(const inode_t* cur){
    const filesystem* fs=cur->fs;
    uint32_t ret=cur-fs->inodes;
    Assert(fs->inodes+ret==cur,"get_id returns a wrong number");
    return ret;
}

static inline void add_inode(const filesystem* fs,const uint32_t id,const yls_node* node){
    *(yls_node*)fs->inodes[id].ptr=*node;
    device_t* dev=fs->dev;
    dev->ops->write(dev,INODE_START+id*sizeof(*node),node,sizeof(*node));
}

static inline inode_t* new_direc(
        const inode_t* cur,uint32_t offset,const char* filename,int flags){
    const filesystem* fs=cur->fs;
    device_t* dev=fs->dev;

    ssize_t(*const write)(device_t*,off_t,const void*,size_t)=fs->dev->ops->write;

    uint32_t off=new_block(dev),inode=new_inode(dev);
    yls_node file={
        .refcnt=1,
        .info  =new_block(dev),
        .size  =0,
        .type  =YLS_DIR,
    };
    write(dev,offset,&off,4);

    int id=(inode-INODE_START)/0x10;
    add_inode(fs,id,&file);

    write(dev,off,&id,4);
    write(dev,off+4,filename,strlen(filename));
    uint32_t parent_id=get_id(cur);
    write(dev,file.info,&parent_id,4);
    return fs->inodes+id;
}

static inline inode_t* new_file(
        const inode_t* cur,uint32_t offset,const char* filename,int flags){
    const filesystem* fs=cur->fs;
    device_t* dev=fs->dev;

    ssize_t(*const write)(device_t*,off_t,const void*,size_t)=fs->dev->ops->write;

    uint32_t off=new_block(dev),inode=new_inode(dev);
    yls_node file={
        .refcnt=1,
        .info  =new_block(dev),
        .size  =0,
        .type  =YLS_FILE,
    };
    write(dev,offset,&off,4);

    int id=(inode-INODE_START)/0x10;
    add_inode(fs,id,&file);

    write(dev,off,&id,4);
    write(dev,off+4,filename,strlen(filename));
    return fs->inodes+id;
}

int is_dir(inode_t* inode){
    yls_node* node=inode->ptr;
    return node->type==YLS_DIR;
}

static inode_t* blkfs_ifind(inode_t* cur,const char* path,int flags){
    inode_t* next=NULL;
    check(cur,path,flags);

    const filesystem* fs=cur->fs;
    device_t* dev=fs->dev;
    yls_node* node=cur->ptr;
    uint32_t offset=node->info;//Skip parent

    ssize_t(*const read )(device_t*,off_t,      void*,size_t)=fs->dev->ops->read;

    uint32_t id=-1;//id of inode

    if(path[0]=='.'){
        if(path[1]=='.'){
            //.. for parent
            if(cur==cur->fs->root){
                next=cur->fs->root_parent;
            }else{
                read(dev,offset,&id,4);
                next=fs->inodes+id;
            }
            path+=2;
        }else{
            //. for current
            next=cur;
            ++path;
        }
    }else{
        offset+=4;
        char layer[0x100];
        int layer_len=get_first_layer(path);
        strncpy(layer,path,layer_len);
        while(offset){
            uint32_t blk_off;
            if(read(fs->dev,offset,&blk_off,4)!=4||!blk_off){
                //No more file in this directory
                if( (flags&O_CREATE) && (path[layer_len]=='\0')){
                    if(flags&O_DIRECTORY)//Only mkdir will gets here
                        return new_direc(cur,offset,path,flags);
                    else
                        return new_file (cur,offset,path,flags);
                }else{
                    warn("No such file or directory");
                    return NULL;
                }
            }
            if(blk_off==YLS_WIPE||block_cmp(fs->dev,blk_off,layer)){
                //Mismatch
                offset+=4;
                if(offset%BLK_SZ==BLK_MEM){
                    //Find next offset
                    read(fs->dev,offset,&offset,4);
                }
            }else{
                path+=layer_len;
                read(fs->dev,blk_off,&id,4);
                next=fs->inodes+id;
                break;
            }
        }
    }

    return next->ops->find(next,path,flags);
}

static int blkfs_ilink(inode_t* parent,const char* name,inode_t* inode){
    const filesystem* fs=parent->fs;
    device_t* dev=fs->dev;
    yls_node* node=parent->ptr;
    ssize_t(*const write)(device_t*,off_t,const void*,size_t)=fs->dev->ops->write;

    uint32_t off=new_block(dev),id=get_id(inode);

    uint32_t offset=find_empty(dev,node->info+4);
    write(dev,offset,&off,4);

    write(dev,off,&id,4);
    write(dev,off+4,name,strlen(name));

    return 0;
}

static int blkfs_iunlink(inode_t* parent,const char* name){
    const filesystem* fs=parent->fs;
    device_t* dev=fs->dev;
    yls_node* node=parent->ptr;
    uint32_t offset=node->info;//Skip parent

    ssize_t(*const read )(device_t*,off_t,      void*,size_t)=dev->ops->read;
    ssize_t(*const write)(device_t*,off_t,const void*,size_t)=fs->dev->ops->write;

    if(name[0]=='.'){
        return -1;
    }else{
        offset+=4;
        while(offset){
            uint32_t blk_off;
            if(read(dev,offset,&blk_off,4)!=4||!blk_off){
                //No more file in this directory
                warn("No such file or directory");
                return -1;
            }
            if(blk_off==YLS_WIPE||block_cmp(dev,blk_off,name)){
                //Mismatch
                offset+=4;
                if(offset%BLK_SZ==BLK_MEM){
                    //Find next offset
                    read(dev,offset,&offset,4);
                }
            }else{
                uint32_t id,type;
                read(dev,blk_off,&id,4);
                read(dev,INODE_START+id*sizeof(yls_node)+offsetof(yls_node,type),&type,4);
                if(type==YLS_DIR){
                    warn("It's a directory!");
                    return -1;
                }else{
                    //TODO: refcnt
                    uint32_t wipe=YLS_WIPE;
                    write(dev,offset,&wipe,4);
                    return 0;
                }
            }
        }
    }

    return -1;
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
    */
    .rmdir  =blkfs_irmdir,
    .link   =blkfs_ilink,
    .unlink =blkfs_iunlink,
    .find   =blkfs_ifind,
};

filesystem blkfs[2]={
    {
        .name    =NULL,
        .ops     =&blkfs_ops,
    },
    {
        .name    =NULL,
        .ops     =&blkfs_ops,
    },
};
