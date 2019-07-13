#ifndef __YLS_H
#define __YLS_H

typedef union yls_node yls_node;
typedef struct block block;

int file_cmp        (device_t* dev,uint32_t off,const char* s   );
int string_cmp      (device_t* dev,uint32_t off,const char* s   );
int block_write     (device_t* dev,uint32_t off,uint32_t shift,const char* s   ,size_t size);
int block_read      (device_t* dev,uint32_t off,uint32_t shift,char* s         ,size_t size);
uint32_t new_block  (device_t* dev,uint32_t size                );
uint32_t find_end   (device_t* dev,uint32_t off                 );
uint32_t find_block (device_t* dev,uint32_t off,uint64_t* fd_off);

int find_path   (device_t* dev,inode_t* cur,const char* s);

#define HEADER_LEN 0x100

#define YLS_DIR  0
#define YLS_FILE 1
#define YLS_MNT  2

union yls_node{
    struct{
        uint32_t refcnt;
        uint32_t info;
        uint32_t size;
        uint32_t type;
    };
    uint8_t align[0x10];
};

#define BLK_MEM (0x80-4)
struct block{
    char mem[BLK_MEM];
    uint32_t next;
};
#define BLK_SZ sizeof(block)

#define OFFS_PER_MEM (BLK_MEM/4)

#endif//__YLS_H
