#include <devices.h>
#include <common.h>
#include <yls.h>
#include <dir.h>

int find_path(device_t* dev,yls_node* cur,const char* path){
    int ret=0;//Starts from "/"
    ssize_t(*const read)(device_t*,off_t,void*,size_t)=dev->ops->read;

    while(path[ret]){
        uint32_t off=cur->info;
        if(cur->type!=YLS_DIR){
            fprintf(2,"%s: not a directory\n",path);
        }
        while(1){
            for(int i=0;i<OFFS_PER_MEM;++i,off+=4){
                uint32_t next_off;
                //Get next's yls_node from off
                read(dev,off,&next_off,4);
                if(!file_cmp(dev,next_off,path)){
                    int len=get_first_slash(path);
                    read(dev,next_off,cur,12);
                    if(len==-1){
                        path    +=strlen(path);
                    }else{
                        path    +=len+1;
                    }
                    goto found;
                }
            }
            read(dev,off,&off,4);
            if(!off){
                return ret;
            }
        }
found:;
    }
    return ret;
}
