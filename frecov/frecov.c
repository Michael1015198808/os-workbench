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

#define offset_of(member,struct) ((uintptr_t)&(((struct*)0)->member))
#define pstruct struct __attribute__((packed))
#define my_cmp(pat, ptr) strncmp(pat,(const char*)ptr,sizeof(pat))
#define len(Array) (sizeof(Array)/sizeof(Array[0]))
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
_Static_assert(offset_of(sector_per_fat_low,bpb_t)==0x16-0xb,"Offset of sector_per_fat_low is wrong!");
_Static_assert(offset_of(sector_per_fat_high,bpb_t)==0x24-0xb,"Offset of sector_per_fat_high is wrong!");
_Static_assert(sizeof(bpb_t)==79,"Size of bpb is wrong!");
uint32_t sector_per_fat(const bpb_t *p){
    return p->sector_per_fat_low==0?p->sector_per_fat_high:p->sector_per_fat_low;
}

int main(int argc, char *argv[]) {
    if(argc!=2){
        fprintf(stderr,"Usage: frecov [file]\n");
        return -1;
    }
    int fd = open(argv[1], O_RDONLY);
    struct stat st;
    fstat(fd, &st);
    const bpb_t *fs = 0xb+mmap(NULL, st.st_size, PROT_READ , MAP_SHARED, fd, 0);

    (void)fs;
    entry_t *e=(entry_t*)(uintptr_t)(((void*)fs)-0xb+
                ( fs->sectors_reserved+
                  fs->fat_cnt*sector_per_fat(fs)+
                  (fs->start_cluster-2)*fs->sectors_per_cluster )
                    *fs->bytes_per_sector);
    entry_t *end=(entry_t*)(uintptr_t)((
                (void*)fs)-0xb+
                fs->sectors_cnt_high*
                fs->bytes_per_sector);
    void *begin=((void*)e)-2*fs->sectors_per_cluster;
    while(e<end){
        if(e->attr==0xf){
            long_entry_t *tmp=(void*)e;
            int idx=0;
            char file_name[50];
            while(tmp->mark==0xf){
                ++tmp;
            }
            printf("tmp:%p\n",tmp);
            entry_t *old_e=e;
            e=(entry_t*)tmp;
            printf("e:%p\n",e);
            do{
                --tmp;
#define print_file_name \
                do{ \
                    for(int i=0;i<len(tmp->NAME);++i){ \
                        file_name[idx++]=tmp->NAME[i]; \
                        if(tmp->NAME[i]==EOF){ \
                            file_name[idx-1]='\0'; \
                            goto outer; \
                        } \
                    } \
                }while(0)
#define NAME name1
                print_file_name;
#undef NAME
#define NAME name2
                print_file_name;
#undef NAME
#define NAME name3
                print_file_name;
#undef NAME
            }while((void*)tmp!=(void*)old_e);
outer:;
            puts(file_name);
            printf("e:%p\n",e);
            printf("high:%x low:%x\n",e->clus_high,e->clus_low);
            printf("%llx %x\n",((e->clus_high*1LL<<32)+e->clus_low),fs->bytes_per_sector);
            uint8_t* file=begin+((e->clus_high*1LL<<32)+e->clus_low)*fs->bytes_per_sector;
            printf("%llx",((uintptr_t)file-(uintptr_t)fs)+0xbLL);
            for(uint32_t i=0;i<e->size;++i){
                putchar(file[i]);
            }
            return 0;
        };
        ++e;
    }
    close(fd);
    return 0;
}
