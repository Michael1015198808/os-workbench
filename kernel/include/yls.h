#ifndef __YLS_H
#define __YLS_H

typedef union yls_node yls_node;
typedef struct block block;

int block_cmp       (device_t* dev,uint32_t off,const char* s   );
int string_cmp      (device_t* dev,uint32_t off,const char* s   );
int block_write     (device_t* dev,uint32_t off,uint32_t shift,const char* s   ,size_t size);
int block_read      (device_t* dev,uint32_t off,uint32_t shift,char* s         ,size_t size);
uint32_t new_block  (device_t* dev);
uint32_t new_inode  (device_t* dev);
uint32_t find_end   (device_t* dev,uint32_t off                 );
uint32_t find_empty (device_t* dev,uint32_t off                 );
int   find_block    (device_t* dev,uint32_t*fd_off,uint32_t* off);
uint32_t block_len  (device_t* dev,uint32_t off);

int find_path   (device_t* dev,inode_t* cur,const char* s);
int yls_init(device_t*);

#define HEADER_OFF 0x40
#define INODE_START 0x40
#define INODE_END 0x400

#define YLS_DIR  0
#define YLS_FILE 1
#define YLS_MNT  2
#define YLS_WIPE 0x3f3f3f3f

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
