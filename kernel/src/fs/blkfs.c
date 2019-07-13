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
    fs->name=name;
    fs->dev=dev;
    log("blkfs initialization started");
    fs->inodes=pmm->alloc(sizeof(inode_t)*(0x100-0x40)/0x8);
    for(int i=0;40+(i<<3)<0x100;++i){
        fs->inodes[i].ptr=pmm->alloc(16);
        fs->dev->ops->read(fs->dev,40+(i<<3),fs->inodes[i].ptr,16);
        fs->inodes[i].fs=fs;
        fs->inodes[i].ops=fs->inodeops;
    }
    log("blkfs initialization finished");
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
    pmm->free(inode->ptr);
    pmm->free(inode);
    return 0;
}

static fsops_t blkfs_ops={
    .init  =blkfs_init,
    .lookup=blkfs_lookup,
    .close =blkfs_close,
};

static int blkfs_iclose(vfile_t* file){
    --file->refcnt;
    if(file->refcnt==0){
        return file->inode->fs->ops->close(file->inode);
    }
    return 0;
}

static ssize_t inline blkfs_iread_real(vfile_t* file,char* buf,size_t size){
    ssize_t ret=0;

    filesystem* fs  =file->inode->fs;
    uint64_t fd_off =file->offset;
    yls_node* node  =file->inode->ptr;
    uint32_t off=node->info;

    switch(node->type){
        case YLS_DIR:
            {
                //Read a directory, you'll get
                //names of directorys in it
                //(So readdir is not needed)
                if(block_read(fs->dev,off,fd_off,(void*)&off,4)!=4)return 0;
                if(off==0)return 0;//find_block failed or reached the end

                ssize_t nread=block_read(fs->dev,off,4,buf,size);
                ret+=nread;
                file->offset+=4;
                return ret;
            }
            break;
        case YLS_FILE:
            {
                find_block(fs->dev,off,&fd_off);
                return block_read(fs->dev,off,fd_off,buf,size);
            }
            break;
    }
    Assert(0,"Should not reach here!\n");
    return 0;
}
static ssize_t blkfs_iread(vfile_t* file,char* buf,size_t size){
    ssize_t ret=blkfs_iread_real(file,buf,size);
    return ret;
}
static ssize_t blkfs_iwrite(vfile_t* file,const char* buf,size_t size){
    filesystem* fs= file->inode->fs;
    uint64_t fd_off =file->offset;
    yls_node* node= file->inode->ptr;
    uint32_t off=node->info;

    switch(node->type){
        case YLS_DIR:
            {
                TODO();
                //Write to a dir will create
                //a file in it with name $buf
                return size;
            }
            break;
        case YLS_FILE:
            {
                find_block(fs->dev,off,&fd_off);
                ssize_t nwrite=block_write(fs->dev,off,fd_off,buf,size);
                file->offset+=nwrite;
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

static inodeops_t blkfs_iops={
    /*
    .open  =blkfs_iopen,
    */
    .close =blkfs_iclose,
    .read  =blkfs_iread,
    .write =blkfs_iwrite,
    .lseek =blkfs_ilseek,
    /*
    .mkdir =blkfs_imkdir,
    .rmdir =blkfs_rmdir,
    .link  =blkfs_ilink,
    .unlink=blkfs_iunlink,
    */
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
