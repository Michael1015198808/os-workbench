#include <devices.h>
#include <dir.h>
#include <yls.h>
#include <fs.h>

inodeops_t yls_iops;

static void yls_init(struct filesystem *fs,const char* name,device_t* dev){
    return;
}

inode_t *yls_lookup(struct filesystem* fs, const char* path, int flags){
    Assert(path[0]=='/',"Absolute path should start with /\n");
    int path_len=strlen(path)-1;//Starts from "/"
    ssize_t(*const read)(device_t*,off_t,void*,size_t)=fs->dev->ops->read;
    ++path;

    yls_node *cur=pmm->alloc(sizeof(yls_node));
    read(fs->dev,HEADER_LEN,cur,12);

    while(path_len>0){
        uint32_t off=cur->info;
        if(cur->type!=YLS_DIR){
            fprintf(2,"%s: not a directory\n",path);
        }
        while(1){
            for(int i=0;i<OFFS_PER_MEM;++i,off+=4){
                uint32_t next_off;
                //Get next's yls_node from off
                read(fs->dev,off,&next_off,4);
                if(!file_cmp(fs->dev,next_off,path)){
                    int len=get_first_slash(path);
                    read(fs->dev,next_off,cur,12);
                    if(len==-1){
                        path_len=0;
                    }else{
                        path_len-=len;
                        path    +=len;
                    }
                    goto found;
                }
            }
            read(fs->dev,off,&off,4);
            if(!off){
                fprintf(2,"%s: No such file or directory\n",path);
                exit();
            }
        }
found:;
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
