#include <assert.h>
#include <stddef.h>
#include <sys/types.h>

#define entry(member,offset) \
    _Static_assert( \
            offsetof(header,member)==offset, \
            "offset of " #member " is wrong!") 

#define off_len sizeof(off_t)

typedef struct{
    struct{
        off_t head,tail;
    }free_list;
}header;
//Since sometimes we only change a little members, this only works as an index;

_Static_assert(sizeof(header)<=HEADER_LEN,"size of header is larger than macro HEADER_LEN!");
entry(free_list.head,0);
entry(free_list.tail,off_len);

#undef entry
#undef off_len
