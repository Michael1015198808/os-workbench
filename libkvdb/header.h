#include <assert.h>
#include <stddef.h>
#include <sys/types.h>

typedef struct{
    struct{
        off_t head,tail;
    }free_list;
}header;
//Since sometimes we only change a little members, this works as an index;
_Static_assert(sizeof(header)<=HEADER_LEN,"size of header is larger than macro HEADER_LEN!");
_Static_assert(offsetof(header,free_list.head)==0,"size of header is larger than macro HEADER_LEN!");
_Static_assert(offsetof(header,free_list.tail)==sizeof(off_t),"size of header is larger than macro HEADER_LEN!");
