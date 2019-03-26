#ifndef __COMMON_H__

#include <kernel.h>
#include <nanos.h>

#ifdef DEBUG
void show_free_list(void);
uintptr_t cnt_free_list(void);
void show_free_pages(void);
#endif
#endif
