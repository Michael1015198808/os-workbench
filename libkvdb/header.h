#include <assert.h>
#include <stddef.h>
#include <sys/types.h>

typedef struct{
    struct{
        off_t head,tail;
    }free_list;
}header;
//Since sometimes we only change a little members, this works as an index;
_Static_assert(1<2,"size of header is larger than macro HEADER_LEN!");
_Static_assert(sizeof(header)<=HEADER_LEN,"size of header is larger than macro HEADER_LEN!");
_Static_assert(sizeof(header)<=HEADER_LEN,"size of header is larger than macro HEADER_LEN!");
