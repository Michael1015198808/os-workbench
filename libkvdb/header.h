#include <assert.h>
#include <stddef.h>
#include <sys/types.h>

#define entry(member,offset) \
    _Static_assert( \
            offsetof(header,member)==offset, \
            "offset of " #member " is wrong!") 

#define off_len sizeof(uint32_t)
#define header_off(member) offsetof(header,member)

typedef struct{
    struct{
        uint32_t head,tail,size;
    }free_list,backup_list;
    tab backup_tab;
    uint32_t pos;
    uint32_t backup_prev;
    uint8_t backup_flag;
}header;
//Since sometimes we only change a little members, this only works as an index;

_Static_assert(sizeof(header)<=HEADER_LEN,"size of header is larger than macro HEADER_LEN!");
entry( free_list.head , 0*off_len);
entry( free_list.tail , 1*off_len);
entry( free_list.size , 2*off_len);
entry( backup_list    , 3*off_len);
entry( backup_tab     , 6*off_len);
entry( pos            ,11*off_len);
entry( backup_prev    ,12*off_len);
entry( backup_flag    ,13*off_len);

#undef entry
#undef off_len
