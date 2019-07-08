#include <dir.h>
#include <common.h>

int get_first_slash_from(const char* const path,int idx){
    for(;path[idx];++idx){
        if(path[idx]=='/')return idx;
    }
    return -1;
}
int get_first_slash(const char* const path){
    return get_first_slag_from(path,0);
}
int get_last_slash_from(const char* const path,int idx){
    for(;idx>0;--idx){
        if(path[idx]=='/')return idx;
    }
    return -1;
}
int get_last_slash(const char* const path){
    return get_last_slash_from(path,strlen(path));
}
void dir_cat(char* dest,const char* src){
    int len=strlen(dest);
    if(dest[len-1]!='/'){
        dest[len-1]='/';
    }
    while(src){
        int next=get_first_slash(src);
        switch(next){
            case -1:
                strcat(dest,src);
                return;
            case 0:
                Assert(0,"Should not reach here!\n");
                break;
            case 1:
                if(strncmp(src,".",1)){
                    goto default;
                }
                break;
            case 2:
                if(strncmp(src,"..",2)){
                    goto default;
                }else{
                    dest[get_last_slash(dest)]='\0';
                }
                break;
            default:
                strncat(dest,src,next+1);
                break;
        }
        src+=(next+1);
    }
}
