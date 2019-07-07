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
        uint32_t ptr;
        uint32_t name;
    };
    uint8_t align[0x10];
};

struct info{
    uint32_t next;
    char mem[0x40-4];
};

#endif//__YLS_H
