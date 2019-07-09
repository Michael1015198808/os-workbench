#include <dir.h>
#include <common.h>

int get_first_slash_from(const char* const path,int idx){
    for(;path[idx];++idx){
        if(path[idx]=='/')return idx;
    }
    return -1;
}
int get_first_slash(const char* const path){
    return get_first_slash_from(path,0);
}
int get_first_layer(const char* const path){
    int ret=get_first_slash(path);
    if(ret==-1){
        return strlen(path);
    }else{
        return ret;
    }
}
int get_last_slash_from(const char* const path,int idx){
    for(;idx>=0;--idx){
        if(path[idx]=='/')return idx;
    }
    return -1;
}
int get_last_slash(const char* const path){
    return get_last_slash_from(path,strlen(path));
}
static inline void dir_cat_real(char* dest,const char* src){
    int end=1;
    while(end){
        int next=get_first_slash(src),flag=0;
        if(next==-1)end=0,next=strlen(src);
        switch(next){
            case -1:
                return;
            case 0:
                break;
            case 1:
                if(strncmp(src,".",1)){
                    flag=1;
                }
                break;
            case 2:
                if(strncmp(src,"..",2)){
                    flag=1;
                }else{
                    if(strcmp(dest,"/")){
                        dest[get_last_slash(dest)]='\0';
                        dest[get_last_slash(dest)+1]='\0';
                    }
                }
                break;
            default:
                flag=1;
                break;
        }
        if(flag){
            strncat(dest,src,next+1);
        }
        src+=(next+1);
    }
}
void dir_cat(char* dest,const char* src){
    Assert(src[0]!='/',"Relative location shouldn't start with /");
    int len=-1;

    //Add '/' to end
    len=strlen(dest);
    if(dest[len-1]!='/'){
        dest[len]='/';
        dest[len+1]='\0';
    }
    dir_cat_real(dest,src);

    //Remove '/' from end
    len=strlen(dest);
    if(len!=1&&dest[len-1]=='/'){
        dest[len-1]='\0';
    }
}
void to_absolute(char* dest,const char* pwd,const char* rela){
    if(rela[0]=='/'){
        //Absolute
        strcpy(dest,rela);
    }else{
        //Relative
        strcpy(dest,pwd);
        dir_cat(dest,rela);
    }
}

inline int find_path(device_t* dev,const char* path,yls_node* cur){
    int ret,path_len=strlen(path)-1;//Starts from "/"
    ssize_t(*const read)(device_t*,off_t,void*,size_t)=fs->dev->ops->read;

    while(ret=0;path[ret];++ret){
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
                        path_len=0;
                    }else{
                        path_len-=len+1;
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
