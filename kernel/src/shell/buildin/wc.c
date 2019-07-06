#include <klib.h>
#include <devices.h>

inline void single_wc(int fd, char *name,int cnt[3]){
    char buf[0x200];
    int i,n;
    int l=0,w=0,c=0,inword=0;

    while((n = std_read(fd, buf, sizeof(buf))) > 0){
        for(i=0; i<n; i++){
            c++;
            if(buf[i] == '\n')
                l++;
            if(strchr(" \r\t\n\v", buf[i]))
                inword = 0;
            else if(!inword){
                w++;
                inword = 1;
            }
        }
    }
    if(n < 0){
        std_write(1, "wc: read error\n");
        exit();
    }
    std_write(1, "%4d %4d %4d %s\n", l, w, c, name);
}

int mysh_wc(char *argv[]){
    int cnt[3]={};

    if(argv[1]){
        for(int i = 1; i < argc; i++){
            if((fd=vfs->open(argv[i],0))<0){
                printf(1, "wc: cannot open %s\n", argv[i]);
                return -1;
            }
            wc(fd, argv[i]);
            close(fd);
        }
        for(int i=0;i<3;++i){
            sprintf(info,"%4d ",cnt[i]);
            std_write(info);
        }
        std_write("total\n");
    }else{
        wc(0, "");
    }
    return 0;
}
