#include <devices.h>
#include <yls.h>
#include <fs.h>

inodeops_t yls_iops;

static void yls_init(struct filesystem *fs,const char* name,device_t* dev){
    return;
}

inode_t *yls_lookup(struct filesystem* fs, const char* path, int flags){
    int path_len=strlen(path)-1;//Starts from "/"
    ssize_t(*read)(device_t*,off_t,void*,size_t);read=fs->dev->ops->read;
    ++path;

    yls_node *cur=pmm->alloc(sizeof(yls_node));
    uint32_t off=cur->info;

    while(path_len>0){
        if(cur->type!=YLS_DIR){
            const char* const NOT_DIR="not a directory:";
            vfs->write(2,(char*)NOT_DIR,sizeof(NOT_DIR));
            vfs->write(2,(char*)path,strlen(path));
        }
        uint32_t next_off=0;
        while(1){
            for(int i=0;i<OFFS_PER_MEM;++i){
                read(fs->dev,HEADER_LEN+off,&next_off,4);//Get next's yls_node
                uint32_t next=0;
                read(fs->dev,HEADER_LEN+next_off+8,&next,4);//Get next's name's offset
                char name[0x40-4];
                read(fs->dev,HEADER_LEN+next,name,4);//Get next's name
                if(strncmp(name,path,strlen(path))){
                    path_len-=strlen(path);
                    path    +=strlen(path);
                    off=next_off;
                    goto found;
                }
                off+=4;
            }
            read(fs->dev,HEADER_LEN+off,&next_off,4);
            if(next_off){
                off=next_off;
            }else{
                const char* const NO_FILE=": No such file or directory\n";
                vfs->write(2,(char*)path,path_len);
                vfs->write(2,(char*)NO_FILE,sizeof(NO_FILE));
            }
        }
found:;
    }
    read(fs->dev,HEADER_LEN,cur,12);

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
                printf("yls_iread reads from %x\n",node->info);
                if(fs->dev->ops->read(fs->dev,node->info,&off,4)!=4)return -1;
                if(off==0)return 0;
                printf("find a file at %x\n",off);
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
