#include <klib.h>
#include <devices.h>

inline void single_wc(int fd, char *name,int total[3]){
    char buf[0x200];
    int i,n;
    int cnt[3],inword=0;
    //cnt[3]:=(l,w,c)
    //line, word, character

    while((n = std_read(fd, buf, sizeof(buf))) > 0){
        for(i=0;i<n;++cnt[2],++i){
            cnt[0]+=(buf[i]=='\n');

            if(strchr(" \r\t\n\v", buf[i]))
                inword = 0;
            else if(!inword){
                ++cnt[1];
                inword = 1;
            }
        }
    }
    if(n < 0){
        vfs->write(2, winfo("wc: read error\n"));
        exit();
    }
    std_write(1, "%4d %4d %4d %s\n", l, w, c, name);
    for(int i=0;i<3;++i){
        total[i]+=cnt[i];
    }
}

int mysh_wc(char *argv[]){
    int cnt[3]={};

    if(argv[1]){
        for(int i = 1;argv[i]; i++){
            int fd=-1;
            if((fd=vfs->open(argv[i],0))<0){
                printf(1, "wc: cannot open %s\n", argv[i]);
                return -1;
            }
            single_wc(fd, argv[i],cnt);
            vfs->close(fd);
        }
        for(int i=0;i<3;++i){
            sprintf(info,"%4d ",cnt[i]);
            std_write(info);
        }
        std_write("total\n");
    }else{
        single_wc(0, "",cnt);
    }
    return 0;
}
