/* 
 * Author: Michael Yan
 * STUID: 171240518
 * Features:
 *      Auto guess whether a bmp file is homo-color, if so, auto compele its color 
 * Fancy designs:
 *      declare a zeros array to write 00...00 faster.
 *      Macro around Line 200 to print file's name
 *      full_file_name and file_name
 * 
 * Tried to fix bmp cross sectors but failed
 */
#include <assert.h>
#include <dlfcn.h>
#include <fcntl.h>
#include <inttypes.h>
#include <readline/history.h>
#include <readline/readline.h>
#include <regex.h>
#include <setjmp.h>
#include <signal.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <time.h>
#include <unistd.h>

//#define LOCAL
#define HOMOCOLOR_HYPOTHESIS
#define SIG_TRAP asm volatile("int $3")
#define offset_of(member,struct) ((uintptr_t)&(((struct*)0)->member))
#define pstruct struct __attribute__((packed))
#define my_cmp(pat, ptr) strncmp(pat,(const char*)ptr,sizeof(pat))
#define len(Array) (sizeof(Array)/sizeof(Array[0]))

const uint8_t zeros[16]={};
inline void print_unicode(uint16_t c){
    if(c>>8){
        putchar(c>>8);
    }
    putchar(c);
}

typedef union long_entry{
    uint8_t info[0];
    pstruct{
        uint8_t attr;
        uint16_t name1[5];
        uint8_t mark,system,veri;
        uint16_t name2[6];
        uint16_t start_cluster;
        uint16_t name3[2];
    };
}long_entry_t;
_Static_assert(sizeof(long_entry_t)==0x20,"Size of long entry is wrong!");

typedef union entry{
    uint8_t info[0];
    pstruct{
        uint16_t file_name[4];
        uint8_t suffix[3];
        uint8_t attr,unused,creat;
        uint16_t creat_time,creat_date,last_access,
                 clus_high,last_edit_time,last_edit_date,clus_low;
        uint32_t size;
    };
}entry_t;
_Static_assert(sizeof(entry_t)==0x20,"Size of entry is wrong!");

typedef union bpb{
    uint8_t info[0];
    pstruct{
        uint16_t bytes_per_sector;
        uint8_t sectors_per_cluster;
        uint16_t sectors_reserved;
        uint8_t fat_cnt;
        uint8_t unused1[2];
        uint16_t sectors_cnt_low;
        uint8_t unused2[1];
        uint16_t sector_per_fat_low;
        uint8_t unused3[8];
        uint32_t sectors_cnt_high;
        uint32_t sector_per_fat_high;
        uint8_t unused4[4];
        uint32_t start_cluster;
        uint8_t unused_[42];
    };
}bpb_t;
const bpb_t *fs=NULL;
_Static_assert(offset_of(sector_per_fat_low,bpb_t)==0x16-0xb,"Offset of sector_per_fat_low is wrong!");
_Static_assert(offset_of(sector_per_fat_high,bpb_t)==0x24-0xb,"Offset of sector_per_fat_high is wrong!");
_Static_assert(sizeof(bpb_t)==79,"Size of bpb is wrong!");

uint32_t sector_per_fat(const bpb_t *p){
    return p->sector_per_fat_low==0?p->sector_per_fat_high:p->sector_per_fat_low;
}

void *disk;
intptr_t get_off(void *p){
    return (intptr_t)p-(intptr_t)disk;
}

typedef union bmp{
    uint8_t info[0];
    pstruct{
        pstruct {
            uint16_t type;
            uint32_t size;
            uint16_t unused[2];
            uint32_t offset;
        }bfh;
        //Bitmap file header
        pstruct{
            uint32_t size;
            uint32_t width,height;
            uint16_t planes;
            uint16_t bit_per_pixel;
        }dibh;
        //DIB header
    };
}bmp_t;
_Static_assert(offset_of(dibh,bmp_t)==14,"Offset of DIB header is wrong!");

int color_test(bmp_t* bmp){
#ifndef HOMOCOLOR_HYPOTHESIS
    return 0;
#else
    uint8_t pixel[3];
    for(int i=0;i<3;++i){
        pixel[i]=bmp->info[bmp->bfh.offset+i];
    }
    uint8_t *current=bmp->info+bmp->bfh.offset;
    for(int i=0;i<bmp->dibh.height;++i){
        for(int j=0;j<bmp->dibh.width;++j){
            for(int k=0;k<3;++k){
                if(*current!=pixel[k]){
                    if((((uintptr_t)current)&(fs->bytes_per_sector-1))<4){
                        return 1;
                    }else{
                        return 0;
                    }
                }
                ++current;
            }
        }
        current+=bmp->dibh.width&3;
    }
    return 1;
#endif
}


#ifdef LOCAL
    #define RECOV_DIREC "./recov/"
#else
    #define RECOV_DIREC "./"
#endif
char full_file_name[70]=RECOV_DIREC;
int main(int argc, char *argv[],char *envp[]) {
    if(argc!=2){
        fprintf(stderr,"Usage: frecov [file]\n");
        return -1;
    }
    int fd = open(argv[1], O_RDONLY);
    struct stat st;
    fstat(fd, &st);
    disk = mmap(NULL, st.st_size, PROT_READ, MAP_SHARED, fd, 0);
    fs=disk+0xb;

    entry_t *e=(entry_t*)(uintptr_t)(disk+
                ( fs->sectors_reserved+
                  fs->fat_cnt*sector_per_fat(fs)+
                  (fs->start_cluster-2)*fs->sectors_per_cluster )*1LL
                    *fs->bytes_per_sector);

    entry_t *end=(entry_t*)(uintptr_t)(
                disk+
                ((fs->sectors_cnt_high*1LL)+fs->sectors_cnt_low)*
                fs->bytes_per_sector);

    void *begin=((void*)e)-(2*fs->sectors_per_cluster*fs->bytes_per_sector);
    for(;e!=end;++e){
        if(e->attr!=0xf){
            continue;
        }else{
            long_entry_t *tmp=(void*)e;
            int idx=0;
            while(tmp->mark==0xf){
                ++tmp;
            }
            entry_t *old_e=e;
            e=(entry_t*)tmp;
            char* file_name=full_file_name+strlen(RECOV_DIREC);
            do{
                --tmp;
#define print_file_name(NAME) \
                do{ \
                    for(int i=0;i<len(tmp->NAME);++i){ \
                        file_name[idx++]=tmp->NAME[i]; \
                        if(tmp->NAME[i]&0x80){ \
                            file_name[idx-1]='\0'; \
                            goto outer; \
                        } \
                    } \
                }while(0)
                print_file_name(name1);
                print_file_name(name2);
                print_file_name(name3);
            }while((void*)tmp!=(void*)old_e);
outer:;
            if( (e->info[0]!=0xe5)&&//Deleted?
                (e->size!=0)&&//Deleted?
                !(strncmp(file_name+strlen(file_name)-4,".bmp",4))&&//.bmp?
                !(strncmp((char*)e->suffix,"BMP",3)) ){//bmp file?

#ifdef LOCAL
                printf("0x%08llx: ",1LL*(((void*)e)-disk));
#endif

                uint8_t* file=begin+((e->clus_high*1LL<<16)+e->clus_low)*fs->bytes_per_sector;
                int recov_file = open(full_file_name, O_WRONLY | O_CREAT, 0777);
                bmp_t* bmp=(bmp_t*)file;
                if(color_test(bmp)){
                    //homo color
#ifdef LOCAL
                    printf("(Homo)");
#endif
                    write(recov_file,file,bmp->bfh.offset);
                    for(int i=0;i<bmp->dibh.height;++i){
                        for(int j=0;j<bmp->dibh.width;++j){
                            write(recov_file,&bmp->info[bmp->bfh.offset],3);
                        }
                        write(recov_file,zeros,(bmp->dibh.width)&3);
                    }
                }else{
                    write(recov_file,file,e->size);
                }
#ifdef LOCAL
                puts(file_name);
#endif
                close(recov_file);
#ifndef LOCAL
                int pid=fork();
                if(pid==0){
                    char *new_argv[3]={"/usr/bin/sha1sum",full_file_name,NULL};
                    execve("/usr/bin/sha1sum",new_argv,envp);
                }else if(pid<0){
                    fprintf(stderr,"Can't fork a thread to calculate sha1sum!\nSee %s:%d for more info.\n",__FILE__,__LINE__);
                    while(1);
                }
#endif
            }
            memset(file_name,0,idx);
        }
    }
    close(fd);
    return 0;
}
