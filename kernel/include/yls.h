#ifndef __YLS_H
#define __YLS_H

typedef union yls_node yls_node;
typedef struct yls_dir yls_dir;
typedef struct yls_file yls_file;
typedef struct info info;

#define HEADER_LEN 0x100

#define YLS_DIR  0
#define YLS_FILE 1
#define YLS_MNT  2

union yls_node{
    struct{
        uint32_t type;
        uint32_t info;
        uint32_t name;
        uint32_t cnt;
    };
    uint8_t align[0x10];
};

struct info{
    char mem[0x40-4];
    uint32_t next;
};
#define OFFS_PER_MEM (sizeof(((info*)0)->mem)/4)

#endif//__YLS_H
